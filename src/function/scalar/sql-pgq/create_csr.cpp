#include "duckdb/function/scalar/sql_pgq_functions.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/common/mutex.hpp"
#include "duckdb/planner/expression/bound_function_expression.hpp"
#include "duckdb/common/vector_operations/unary_executor.hpp"
#include "duckdb/common/vector_operations/binary_executor.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/vector_operations/vector_operations.hpp"
#include "duckdb/execution/expression_executor.hpp"

namespace duckdb {

struct CsrBindData : public FunctionData {
	ClientContext &context;
	int32_t id;
	int32_t vertex_size;

	CsrBindData(ClientContext &context, int32_t id) : context(context), id(id) {
	}

	unique_ptr<FunctionData> Copy() override {
		return make_unique<CsrBindData>(context, id);
	}
};

static void CsrInitializeVertexOrEdge(ClientContext &context, int32_t id, int64_t v_size, int64_t e_size = 0,
                                      bool is_vertex = true) {
	if (is_vertex) {
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
			for (idx_t i = 0; i < (idx_t)v_size + 2; i++) {
				csr->v[i] = 0;
			}
			if (((u_int64_t)id) < context.csr_list.size()) {
				context.csr_list[id] = move(csr);
			} else {
				context.csr_list.push_back(move(csr));
			}
			// dont' forget to destroy
		} catch (std::bad_alloc const &) {
			//  v_size + 2
			throw Exception("Unable to initialise vector of size for csr vertex table representation");
		}
		context.initialized_v = true;
		return;
	} else {
		lock_guard<mutex> csr_init_lock(context.csr_lock);
		if (context.initialized_e) {
			return;
		}
		try {
			context.csr_list[id]->e.resize(e_size, 0);

		} catch (std::bad_alloc const &) {
			throw Exception("Unable to initialise vector of size for csr edge table representation");
		}

		// create running sum
		for (auto i = 1; i < v_size + 2; i++) {
			context.csr_list[id]->v[i] += context.csr_list[id]->v[i - 1];
		}
		context.initialized_e = true;
		return;
	}
}

static void CreateCsrVertexFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

	int64_t input_size = args.data[1].GetValue(0).GetValue<int64_t>();
	if (!info.context.initialized_v) {
		CsrInitializeVertexOrEdge(info.context, info.id, input_size, 0, true);
	}

	BinaryExecutor::Execute<int64_t, int64_t, int64_t>(args.data[2], args.data[3], result, args.size(),
	                                                   [&](int64_t src, int64_t cnt) {
		                                                   int64_t edge_count = 0;

		                                                   info.context.csr_list[info.id]->v[src + 2] = cnt;

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

	return make_unique<CsrBindData>(context, id.GetValue<int32_t>());
}

static void CreateCsrEdgeFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

	int64_t vertex_size = args.data[1].GetValue(0).GetValue<int64_t>();
	int64_t edge_size = args.data[2].GetValue(0).GetValue<int64_t>();
	if (!info.context.initialized_e) {
		CsrInitializeVertexOrEdge(info.context, info.id, vertex_size, edge_size, false);
	}

	BinaryExecutor::Execute<int64_t, int64_t, int32_t>(args.data[3], args.data[4], result, args.size(),
	                                                   [&](int64_t src, int64_t dst) {
		                                                   auto pos = ++info.context.csr_list[info.id]->v[src + 1];
		                                                   info.context.csr_list[info.id]->e[(int64_t)pos - 1] = dst;
		                                                   return 1;
	                                                   });

	return;
}

static unique_ptr<FunctionData> CreateCsrEdgeBind(ClientContext &context, ScalarFunction &bound_function,
                                                  vector<unique_ptr<Expression>> &arguments) {
	if (!arguments[0]->IsFoldable()) {
		throw InvalidInputException("Id must be constant.");
	}

	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);
	return make_unique<CsrBindData>(context, id.GetValue<int32_t>());
}

void CreateCsrFun::RegisterFunction(BuiltinFunctions &set) {
	// params -> id, size, src/dense_id, cnt
	set.AddFunction(ScalarFunction(
	    "create_csr_vertex", {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::BIGINT, CreateCsrVertexFunction, false, CreateCsrVertexBind));
	// params -> id, v_size, num_edges, src_rowid, dst_rowid
	set.AddFunction(ScalarFunction(
	    "create_csr_edge",
	    {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::INTEGER, CreateCsrEdgeFunction, false, CreateCsrEdgeBind));
}

} // namespace duckdb
