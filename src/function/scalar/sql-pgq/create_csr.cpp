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
	// int32_t edge_size;
	// mutex csr_lock;

	CsrBindData(ClientContext &context, int32_t id) : context(context), id(id) {
	}

	// CsrBindData(ClientContext &context, int32_t id, int32_t vertex_size)
	//     : context(context), id(id), vertex_size(vertex_size) {
	// }

	~CsrBindData() {
		// could possibly destroy the array here
	}

	unique_ptr<FunctionData> Copy() override {
		return make_unique<CsrBindData>(context, id);
	}
};

static void csr_initialize_vertex_or_edge(ClientContext &context, int32_t id, int64_t v_size, int64_t e_size = 0,
                                          bool is_vertex = true) {
	// Vector result;
	// auto csr = ((u_int64_t) id) < context.csr_list.size() ? context.csr_list[id] : make_unique<Csr>();
	// auto csr = ((u_int64_t)id) < context.csr_list.size() ? move(context.csr_list[id]) : make_unique<Csr>();
	// unique_pte here ?
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
			if (((u_int64_t)id) < context.csr_list.size())
				context.csr_list[id] = move(csr);
			else
				context.csr_list.push_back(move(csr));

			// dont' forget to destroy
			// }
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
			// csr->e = new std::atomic<int32_t>[e_size + 2];
			context.csr_list[id]->e.resize(e_size, 0);

		} catch (std::bad_alloc const &) {
			throw Exception("Unable to initialise vector of size for csr edge table representation");
		}

		// 	//create running sum
		for (auto i = 1; i < v_size + 2; i++) {
			context.csr_list[id]->v[i] += context.csr_list[id]->v[i - 1];
		}
		context.initialized_e = true;
		// if (((u_int64_t)id) < context.csr_list.size())
		// 	context.csr_list[id] = move(csr);
		// else
		// 	context.csr_list.push_back(move(csr));

		return;
	}
}

static void create_csr_vertex_function(DataChunk &args, ExpressionState &state, Vector &result) {
	// D_ASSERT(args.ColumnCount() == 0);
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

	int64_t input_size = args.data[1].GetValue(0).GetValue<int64_t>();
	if (!info.context.initialized_v) {
		csr_initialize_vertex_or_edge(info.context, info.id, input_size, 0, true);
		// csr_initialize_vertex_or_edge(args, state, true);
	}
	// auto csr = move(info.context.csr_list[info.id]);

	BinaryExecutor::Execute<int64_t, int64_t, int64_t>(args.data[2], args.data[3], result, args.size(),
	                                                         [&](int64_t src, int64_t cnt) {
		                                                         int64_t edge_count = 0;

		                                                         // for(idx_t i = 0; i < src.size(); i++) {
		                                                         // *csr.v[src[i+2]] = 1;
		                                                         info.context.csr_list[info.id]->v[src + 2] = cnt;

		                                                         edge_count = edge_count + cnt;
		                                                         return edge_count;
	                                                         });
	// info.context.csr_list[info.id] = move(csr);
	return;
}

static unique_ptr<FunctionData> create_csr_vertex_bind(ClientContext &context, ScalarFunction &bound_function,
                                                       vector<unique_ptr<Expression>> &arguments) {
	// SequenceCatalogEntry *sequence = nullptr;
	if (!arguments[0]->IsFoldable()) {
		throw InvalidInputException("Id must be constant.");
	}

	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);
	// Value vertex_size = ExpressionExecutor::EvaluateScalar(*arguments[1]);

	return make_unique<CsrBindData>(context, id.GetValue<int32_t>());
	// , vertex_size.GetValue<int32_t>());
}

static void create_csr_edge_function(DataChunk &args, ExpressionState &state, Vector &result) {
	// D_ASSERT(args.ColumnCount() == 0);
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

	int64_t vertex_size = args.data[1].GetValue(0).GetValue<int64_t>();
	int64_t edge_size = args.data[2].GetValue(0).GetValue<int64_t>();
	if (!info.context.initialized_e) {
		csr_initialize_vertex_or_edge(info.context, info.id, vertex_size, edge_size, false);
	}

	// auto csr = move(info.context.csr_list[info.id]);

	BinaryExecutor::Execute<int64_t, int64_t, int32_t>(args.data[3], args.data[4], result, args.size(),
	                                                         [&](int64_t src, int64_t dst) {
		                                                         auto pos = ++info.context.csr_list[info.id]->v[src + 1];
		                                                         info.context.csr_list[info.id]->e[(int64_t)pos - 1] = dst;
		                                                         return 1;
	                                                         });

	// info.context.csr_list[info.id] = move(csr);
	return;
}

static unique_ptr<FunctionData> create_csr_edge_bind(ClientContext &context, ScalarFunction &bound_function,
                                                     vector<unique_ptr<Expression>> &arguments) {
	if (!arguments[0]->IsFoldable()) {
		throw InvalidInputException("Id must be constant.");
	}

	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);
	// Value vertex_size = ExpressionExecutor::EvaluateScalar(*arguments[1]);
	// Value edge_size = ExpressionExecutor::EvaluateScalar(*arguments[2]);

	return make_unique<CsrBindData>(context, id.GetValue<int32_t>());
}

void CreateCsrFun::RegisterFunction(BuiltinFunctions &set) {
	//params -> id, size, src/dense_id, cnt
	set.AddFunction(ScalarFunction(
	    "create_csr_vertex", {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::BIGINT, create_csr_vertex_function, false, create_csr_vertex_bind));
	//params -> id, v_size, num_edges, src_rowid, dst_rowid
	set.AddFunction(ScalarFunction(
	    "create_csr_edge",
	    {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::INTEGER, create_csr_edge_function, false, create_csr_edge_bind));
}

} // namespace duckdb
