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
	int32_t edge_size;
	// mutex csr_lock;

	CsrBindData(ClientContext &context, int32_t id, int32_t vertex_size)
	    : context(context), id(id), vertex_size(vertex_size) {
	}

	CsrBindData(ClientContext &context, int32_t id, int32_t vertex_size, int32_t edge_size)
	    : context(context), id(id), vertex_size(vertex_size), edge_size(edge_size) {
	}

	~CsrBindData() {
	}

	unique_ptr<FunctionData> Copy() override {
		return make_unique<CsrBindData>(context, id, vertex_size);
	}
};

static void csr_initialize_vertex_or_edge(ClientContext &context, int32_t id, int32_t v_size, int32_t e_size = 0,
                                          bool is_vertex = true) {
	Vector result;
	// auto csr = ((u_int64_t) id) < context.csr_list.size() ? context.csr_list[id] : make_unique<Csr>();
	auto csr = ((u_int64_t)id) < context.csr_list.size() ? move(context.csr_list[id]) : make_unique<Csr>();
	// unique_pte here ?
	if (is_vertex) {
		lock_guard<mutex> csr_init_lock(context.csr_lock);
		if (context.initialized_v) {
			return;
		}

		try {
			// extra 2 spaces required for CSR padding
			// data contains a vector of elements so will need an anonymous function to apply the
			// the first element id is repeated across, can I access the value directly?
			csr->v = new std::atomic<int32_t>[v_size + 2];
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
			csr->e.resize(e_size, 0);

		} catch (std::bad_alloc const &) {
			throw Exception("Unable to initialise vector of size for csr edge table representation");
		}

		// 	//create running sum
		for (auto i = 1; i < v_size + 2; i++) {
			csr->v[i] += csr->v[i - 1];
		}
		context.initialized_e = true;
		if (((u_int64_t)id) < context.csr_list.size())
			context.csr_list[id] = move(csr);
		else
			context.csr_list.push_back(move(csr));

		return;
	}
}

static void create_csr_vertex_function(DataChunk &args, ExpressionState &state, Vector &result) {
	// D_ASSERT(args.ColumnCount() == 0);
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

	if (!info.context.initialized_v) {
		csr_initialize_vertex_or_edge(info.context, info.id, info.vertex_size, 0, true);
		// csr_initialize_vertex_or_edge(args, state, true);
	}
	auto csr = move(info.context.csr_list[info.id]);

	BinaryExecutor::Execute<int32_t, int32_t, int32_t, true>(args.data[2], args.data[3], result, args.size(),
	                                                         [&](int32_t src, int32_t cnt) {
		                                                         int32_t edge_count = 0;

		                                                         // for(idx_t i = 0; i < src.size(); i++) {
		                                                         // *csr.v[src[i+2]] = 1;
		                                                         csr->v[src + 2] += cnt;

		                                                         edge_count = edge_count + cnt;
		                                                         return edge_count;
	                                                         });
	info.context.csr_list[info.id] = move(csr);
	return;
}

static unique_ptr<FunctionData> create_csr_vertex_bind(ClientContext &context, ScalarFunction &bound_function,
                                                       vector<unique_ptr<Expression>> &arguments) {
	// SequenceCatalogEntry *sequence = nullptr;

	if (!arguments[0]->IsFoldable() || !arguments[1]->IsFoldable()) {
		throw InvalidInputException("Vertex size and id must be constant.");
	}
	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);
	Value vertex_size = ExpressionExecutor::EvaluateScalar(*arguments[1]);

	return make_unique<CsrBindData>(context, id.GetValue<int32_t>(), vertex_size.GetValue<int32_t>());
}

static void create_csr_edge_function(DataChunk &args, ExpressionState &state, Vector &result) {
	// D_ASSERT(args.ColumnCount() == 0);
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

	if (!info.context.initialized_e) {
		csr_initialize_vertex_or_edge(info.context, info.id, info.vertex_size, info.edge_size, false);
	}

	auto csr = move(info.context.csr_list[info.id]);

	BinaryExecutor::Execute<int32_t, int32_t, int32_t, true>(args.data[3], args.data[4], result, args.size(),
	                                                         [&](int32_t src, int32_t dst) {
		                                                         auto pos = ++csr->v[src + 1];
		                                                         csr->e[(int)pos - 1] = dst;
		                                                         return 1;
	                                                         });

	return;
}

static unique_ptr<FunctionData> create_csr_edge_bind(ClientContext &context, ScalarFunction &bound_function,
                                                     vector<unique_ptr<Expression>> &arguments) {
	if (!arguments[0]->IsFoldable() && !arguments[1]->IsFoldable()) {
		throw InvalidInputException("Id and number of edges must be constant.");
	}

	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);
	Value vertex_size = ExpressionExecutor::EvaluateScalar(*arguments[1]);
	Value edge_size = ExpressionExecutor::EvaluateScalar(*arguments[2]);

	return make_unique<CsrBindData>(context, id.GetValue<int32_t>(), vertex_size.GetValue<int32_t>(),
	                                edge_size.GetValue<int32_t>());
}

struct AddOneOperator {
	template <class TA, class TR>
	static inline TR Operation(TA input) {
		return input + 1;
	}
};

struct BitCntOperatorCsr {
	template <class TA, class TR>
	static inline TR Operation(TA input) {
		using TU = typename std::make_unsigned<TA>::type;
		TR count = 0;
		for (auto value = TU(input); value > 0; value >>= 1) {
			count += TR(value & 1);
		}
		return count;
	}
};

void CreateCsrFun::RegisterFunction(BuiltinFunctions &set) {

	set.AddFunction(ScalarFunction(
	    "create_csr_vertex", {LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::INTEGER},
	    LogicalType::INTEGER, create_csr_vertex_function, false, create_csr_vertex_bind));
	set.AddFunction(ScalarFunction(
	    "create_csr_edge",
	    {LogicalType::INTEGER, LogicalType::HUGEINT, LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::INTEGER},
	    LogicalType::INTEGER, create_csr_edge_function, false, create_csr_edge_bind));
}

} // namespace duckdb
