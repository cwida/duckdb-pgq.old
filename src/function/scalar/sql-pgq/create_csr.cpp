#include "duckdb/common/exception.hpp"
#include "duckdb/common/mutex.hpp"
#include "duckdb/common/vector_operations/binary_executor.hpp"
#include "duckdb/common/vector_operations/unary_executor.hpp"
#include "duckdb/common/vector_operations/vector_operations.hpp"
#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/function/scalar/sql_pgq_functions.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/planner/expression/bound_function_expression.hpp"

#include <iostream>

namespace duckdb {

struct CsrBindData : public FunctionData {
	ClientContext &context;
	int32_t id;
	bool weighted = false;

	CsrBindData(ClientContext &context, int32_t id, bool weighted) : context(context), id(id), weighted(weighted) {
	}

	unique_ptr<FunctionData> Copy() override {
		return make_unique<CsrBindData>(context, id, weighted);
	}
};

static void CsrInitializeVertex(ClientContext &context, int32_t id, int64_t v_size) {
	lock_guard<mutex> csr_init_lock(context.csr_lock);
	if (context.initialized_v) {
		return;
	}
	try {
		auto csr = make_unique<Csr>();
		// extra 2 spaces required for CSR padding
		// data contains a vector of elements so will need an anonymous function to apply the
		// the first element id is repeated across, can I access the value directly?
		csr->v = new std::atomic<int64_t>[v_size + 2];
		csr->v_weight = new std::atomic<int64_t>[v_size + 2];

		for (idx_t i = 0; i < (idx_t)v_size + 2; i++) {
			csr->v[i] = 0;
			csr->v_weight[i] = 0;
		}
		if (((u_int64_t)id) < context.csr_list.size()) {
			context.csr_list[id] = move(csr);
		} else {
			context.csr_list.push_back(move(csr));
		}
	} catch (std::bad_alloc const &) {
		throw Exception("Unable to initialise vector of size for csr vertex table representation");
	}
	context.initialized_v = true;
	return;
}

static void CsrInitializeEdge(ClientContext &context, int32_t id, int64_t v_size, int64_t e_size) {
	lock_guard<mutex> csr_init_lock(context.csr_lock);
	if (context.initialized_e) {
		return;
	}
	try {
		context.csr_list[id]->e.resize(e_size, 0);

	} catch (std::bad_alloc const &) {
		throw Exception("Unable to initialise vector of size for csr edge table representation");
	}

	for (auto i = 1; i < v_size + 2; i++) {
		context.csr_list[id]->v[i] += context.csr_list[id]->v[i - 1];
	}
	context.initialized_e = true;
	return;
}

static void CsrInitializeWeight(ClientContext &context, int32_t id, int64_t v_size, int64_t e_size) {
	lock_guard<mutex> csr_init_lock(context.csr_lock);
	if (context.initialized_w) {
		return;
	}
	try {
		context.csr_list[id]->w.resize(e_size, 0);

	} catch (std::bad_alloc const &) {
		throw Exception("Unable to initialise vector of size for csr weight table representation");
	}

	for (auto i = 1; i < v_size + 2; i++) {
		context.csr_list[id]->v_weight[i] += context.csr_list[id]->v_weight[i - 1];
	}
	context.initialized_w = true;
	return;
}

static void CreateCsrVertexFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

	int64_t input_size = args.data[1].GetValue(0).GetValue<int64_t>();
	if (!info.context.initialized_v) {
		CsrInitializeVertex(info.context, info.id, input_size);
	}
	BinaryExecutor::Execute<int64_t, int64_t, int64_t>(args.data[2], args.data[3], result, args.size(),
	                                                   [&](int64_t src, int64_t cnt) {
		                                                   int64_t edge_count = 0;
		                                                   info.context.csr_list[info.id]->v[src + 2] = cnt;
		                                                   info.context.csr_list[info.id]->v_weight[src + 2] = cnt;
		                                                   edge_count = edge_count + cnt;
		                                                   return edge_count;
	                                                   });
	return;
}

static unique_ptr<FunctionData> CreateCsrVertexBind(ClientContext &context, ScalarFunction &bound_function,
                                                    vector<unique_ptr<Expression>> &arguments) {
	if (!arguments[0]->IsFoldable()) {
		throw InvalidInputException("Id must be constant.");
	}

	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);

	return make_unique<CsrBindData>(context, id.GetValue<int32_t>(), false);
}

static void CreateCsrEdgeFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

	int64_t vertex_size = args.data[1].GetValue(0).GetValue<int64_t>();
	int64_t edge_size = args.data[2].GetValue(0).GetValue<int64_t>();
	if (!info.context.initialized_e) {
		CsrInitializeEdge(info.context, info.id, vertex_size, edge_size);
	}

	BinaryExecutor::Execute<int64_t, int64_t, int32_t>(args.data[3], args.data[4], result, args.size(),
	                                                   [&](int64_t src, int64_t dst) {
		                                                   auto pos = ++info.context.csr_list[info.id]->v[src + 1];
		                                                   info.context.csr_list[info.id]->e[(int64_t)pos - 1] = dst;
		                                                   return 1;
	                                                   });
	if (info.weighted) {
		if (!info.context.initialized_w) {
			CsrInitializeWeight(info.context, info.id, vertex_size, edge_size);
		}
		BinaryExecutor::Execute<int64_t, int64_t, int32_t>(
		    args.data[3], args.data[5], result, args.size(), [&](int64_t src, int64_t weight) {
			    auto pos = ++info.context.csr_list[info.id]->v_weight[src + 1];
			    info.context.csr_list[info.id]->w[(int64_t)pos - 1] = weight;

			    return 1;
		    });
	}

	return;
}

static unique_ptr<FunctionData> CreateCsrEdgeBind(ClientContext &context, ScalarFunction &bound_function,
                                                  vector<unique_ptr<Expression>> &arguments) {
	if (!arguments[0]->IsFoldable()) {
		throw InvalidInputException("Id must be constant.");
	}
	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);
	if (arguments.size() == 5) {
		return make_unique<CsrBindData>(context, id.GetValue<int32_t>(), false);
	} else {
		return make_unique<CsrBindData>(context, id.GetValue<int32_t>(), true);
	}
}

// static void CreateCsrWeightFunction(DataChunk &args, ExpressionState &state, Vector &result) {
//	auto &func_expr = (BoundFunctionExpression &)state.expr;
//	auto &info = (CsrBindData &)*func_expr.bind_info;
//
//	auto v_size = args.data[1].GetValue(0).GetValue<int64_t>();
//	int64_t edge_size = args.data[2].GetValue(0).GetValue<int64_t>();
//	if (!info.context.initialized_w) {
//		CsrInitializeWeight(info.context, info.id, v_size, edge_size);
//	}
//
//	BinaryExecutor::Execute<int64_t, int64_t, int32_t>(
//	    args.data[3], args.data[4], result, args.size(), [&](int64_t src, int64_t weight) {
//		    auto pos = ++info.context.csr_list[info.id]->v_weight[src + 1];
//		    info.context.csr_list[info.id]->w[(int64_t)pos - 1] = weight;
//
//		    return 1;
//	    });
//
//	return;
// }
//
// static unique_ptr<FunctionData> CreateCsrWeightBind(ClientContext &context, ScalarFunction &bound_function,
//                                                     vector<unique_ptr<Expression>> &arguments) {
//	if (!arguments[0]->IsFoldable()) {
//		throw InvalidInputException("Id must be constant.");
//	}
//
//	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);
//	return make_unique<CsrBindData>(context, id.GetValue<int32_t>());
// }

void CreateCsrFun::RegisterFunction(BuiltinFunctions &set) {
	// params -> id, size, src/dense_id, cnt
	set.AddFunction(ScalarFunction(
	    "create_csr_vertex", {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::BIGINT, CreateCsrVertexFunction, false, CreateCsrVertexBind));
	// params -> id, v_size, num_edges, src_rowid, dst_rowid
	set.AddFunction(ScalarFunction("create_csr_edge_and_weight",
	   {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT,
		LogicalType::BIGINT, LogicalType::BIGINT},
	   LogicalType::INTEGER, CreateCsrEdgeFunction, false, CreateCsrEdgeBind));
	set.AddFunction(ScalarFunction(
	    "create_csr_edge",
	    {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::INTEGER, CreateCsrEdgeFunction, false, CreateCsrEdgeBind));

	//	set.AddFunction(ScalarFunction("create_csr_weight",
	//	                               {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT,
	// LogicalType::BIGINT, 	                                LogicalType::BIGINT}, LogicalType::INTEGER,
	// CreateCsrWeightFunction, false, CreateCsrWeightBind));
}

} // namespace duckdb
