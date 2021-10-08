#include "duckdb/function/scalar/sql_pgq_functions.hpp"
#include "duckdb/planner/expression/bound_function_expression.hpp"
#include "duckdb/common/vector_operations/vector_operations.hpp"
#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/common/vector_operations/vector_operations.hpp"

namespace duckdb {

struct MsbfsBindData : public FunctionData {
	ClientContext &context;
	// int32_t num_bfs;

	// MsbfsBindData(ClientContext &context, int32_t num_bfs) : context(context), num_bfs(num_bfs) {
	// }
	MsbfsBindData(ClientContext &context) : context(context) {
	}

	// CsrBindData(ClientContext &context, int32_t id, int32_t vertex_size)
	//     : context(context), id(id), vertex_size(vertex_size) {
	// }

	~MsbfsBindData() {
	}

	unique_ptr<FunctionData> Copy() override {
		return make_unique<MsbfsBindData>(context);
	}
};

// static bool check_empty_bitset(vector<int64_t> set_list) {
// 	bool ans = true;
// 	// int64_t sum = 0;
// 	for(auto set: set_list) {
// 		// sum = sum + set;
// 		if(set > 0) {
// 			ans = false;
// 			break;
// 		}
// 	}
// 	return ans;
// }

static void msbfs_function(DataChunk &args, ExpressionState &state, Vector &result) {
	// D_ASSERT(args.ColumnCount() == 0);
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (MsbfsBindData &)*func_expr.bind_info;

	int64_t num_bfs = args.data[0].GetValue(0).GetValue<int64_t>();
	int64_t input_size = args.data[1].GetValue(0).GetValue<int64_t>();

	auto &src = args.data[2];
	// .GetValue(0).GetValue<int64_t>();

	VectorData vdata_src, vdata_target;
	src.Orrify(args.size(), vdata_src);

	auto src_data = (int64_t *)vdata_src.data;

	auto &target = args.data[3];
	// .GetValue(0).GetValue<int64_t>();
	target.Orrify(args.size(), vdata_target);
	auto target_data = (int64_t *)vdata_target.data;
	// const int32_t bfs = info.num_bfs;

	vector<int64_t> seen(input_size);
	vector<int64_t> visit(input_size);
	vector<int64_t> visit_next(input_size);

	// for (auto idx = 0; idx< )
	for (auto i = 0, idx = 0; i < num_bfs && idx < (int32_t)args.size(); i++, idx++) {
		seen[src_data[idx]] = i + 1;
		visit[src_data[idx]] = i + 1;
	}
	bool init = true;
	int64_t d = 0;
	// while(!check_empty_bitset(visit)) {
	while (d > 0 || init) {
		init = false;
		for (int64_t i = 0; i < input_size - 1; i++) {
			// get vertex list
			// auto v = i;
			if (!visit[i])
				continue;
			auto csr = move(info.context.csr_list[0]);
			// ->e);
			// auto csr_v = move(info.context.csr_list[0]->v);
			for (auto index = (long)csr->v[i]; index < (long)csr->v[i + 1]; index++) {
				auto n = csr->e[index];
				d = visit[i] & ~seen[n];
				if (d > 0) {
					visit_next[n] = visit_next[n] | d;
					seen[n] = seen[n] | d;
				}
			}
			info.context.csr_list[0] = move(csr);
		}
		visit = visit_next;
		for (auto i = 0; i < input_size; i++) {
			visit_next[i] = 0;
		}
		// check target
		// visit_list with n's to keep track (iniitally 64 nodes to visit but looping till input_size)
		// top down vs bottom up ->
		// adaptive with and without visit list-> benchmark
	}

	result.vector_type = VectorType::FLAT_VECTOR;
	auto result_data = FlatVector::GetData<bool>(result);
	// auto &result_mask = FlatVector::Nullmask(result);

	for (idx_t i = 0; i < args.size(); i++) {
		if ((seen[src_data[i]] & seen[target_data[i]]) == 0) {
			result_data[i] = false;
		} else {
			result_data[i] = true;
		}
	}
}

static unique_ptr<FunctionData> msbfs_bind(ClientContext &context, ScalarFunction &bound_function,
                                           vector<unique_ptr<Expression>> &arguments) {
	// 	// SequenceCatalogEntry *sequence = nullptr;
	// 	if (!arguments[0]->IsFoldable()) {
	// 		throw InvalidInputException("Id must be constant.");
	// 	}

	// 	Value num_bfs = ExpressionExecutor::EvaluateScalar(*arguments[0]);

	return make_unique<MsbfsBindData>(context);
}

void MsBfsFun::RegisterFunction(BuiltinFunctions &set) {
	// padding reqd for csr_v ; size unequal
	// w(num_bfs), v_size, source, target
	set.AddFunction(ScalarFunction(
	    "reachability", {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::BOOLEAN, msbfs_function, false, msbfs_bind));
}

} // namespace duckdb