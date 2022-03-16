#include "duckdb/function/scalar/sql_pgq_functions.hpp"
#include "duckdb/main/client_context.hpp"

#include <iostream>

namespace duckdb {

struct PathLengthBindData : public FunctionData {
	ClientContext &context;

	PathLengthBindData(ClientContext &context) : context(context) {
	}

	unique_ptr<FunctionData> Copy() override {
		return make_unique<PathLengthBindData>(context);
	}
};

static int16_t InitialiseBfs(idx_t curr_batch, idx_t size, int64_t *src_data, const SelectionVector *src_sel,
                             const ValidityMask &src_validity, vector<std::bitset<LANE_LIMIT>> &seen,
                             vector<std::bitset<LANE_LIMIT>> &visit, vector<std::bitset<LANE_LIMIT>> &visit_next,
                             unordered_map<int64_t, pair<int16_t, vector<int64_t>>> &lane_map, const uint8_t bfs_depth,
                             unordered_map<int16_t, vector<int8_t>> &depth_map, int64_t input_size) {
	int16_t lanes = 0;
	int16_t curr_batch_size = 0;

	for (idx_t i = curr_batch; i < size && lanes < LANE_LIMIT; i++) {
		auto src_index = src_sel->get_index(i);

		if (src_validity.RowIsValid(src_index)) {
			int64_t &src_entry = src_data[src_index];
			auto entry = lane_map.find(src_entry);
			if (entry == lane_map.end()) {
				lane_map[src_entry].first = lanes;
				// seen[src_data[i]] = std::bitset<LANE_LIMIT>();
				seen[src_entry][lanes] = true;
				// visit[src_data[i]] = std::bitset<LANE_LIMIT>();
				visit[src_entry][lanes] = true;
				depth_map[lanes] = vector<int8_t>(input_size, -1);
				depth_map[lanes][src_entry] = bfs_depth;
				lanes++;
			}
			lane_map[src_entry].second.push_back(i);
			curr_batch_size++;
		}
	}
	return curr_batch_size;
}

static bool BfsWithoutArray(bool exit_early, int32_t id, int64_t input_size, ClientContext &context,
                            vector<std::bitset<LANE_LIMIT>> &seen, vector<std::bitset<LANE_LIMIT>> &visit,
                            vector<std::bitset<LANE_LIMIT>> &visit_next, uint8_t bfs_depth,
                            unordered_map<int16_t, vector<int8_t>> &depth_map) {
	for (int64_t i = 0; i < input_size; i++) {
		if (!visit[i].any()) {
			continue;
		}

		D_ASSERT(context.csr_list[id]);
		for (auto index = (int64_t)context.csr_list[id]->v[i]; index < (int64_t)context.csr_list[id]->v[i + 1];
		     index++) {
			auto n = context.csr_list[id]->e[index];
			visit_next[n] = visit_next[n] | visit[i];
		}
	}

	for (uint64_t bfs_index = 0; bfs_index < visit_next.size(); bfs_index++) {
		if (!visit_next[bfs_index].any()) {
			continue;
		}
		for (uint64_t map_index = 0; map_index < visit_next[bfs_index].size(); map_index++) {
			if (visit_next[bfs_index][map_index]) {
				if (depth_map[map_index][bfs_index] != -1) {
					continue;
				}
				depth_map[map_index][bfs_index] = bfs_depth;
			}
		}
	}

	for (int64_t i = 0; i < input_size; i++) {
		if (visit_next[i].none()) {
			continue;
		}
		visit_next[i] = visit_next[i] & ~seen[i];
		seen[i] = seen[i] | visit_next[i];
		if (exit_early && visit_next[i].any()) {
			exit_early = false;
		}
	}
	return exit_early;
}

static void PathLengthFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (PathLengthBindData &)*func_expr.bind_info;

	int32_t id = args.data[0].GetValue(0).GetValue<int32_t>();
	bool is_variant = args.data[1].GetValue(0).GetValue<bool>();
	int64_t input_size = args.data[2].GetValue(0).GetValue<int64_t>();

	auto &src = args.data[3];

	VectorData vdata_src, vdata_target;
	src.Orrify(args.size(), vdata_src);

	auto src_data = (int64_t *)vdata_src.data;

	auto &target = args.data[4];
	target.Orrify(args.size(), vdata_target);
	auto target_data = (int64_t *)vdata_target.data;

	idx_t result_size = 0;
	//	vector<int64_t> visit_list;
	//	size_t visit_limit = input_size / VISIT_SIZE_DIVISOR;
	//	size_t num_nodes_to_visit = 0;
	result.SetVectorType(VectorType::FLAT_VECTOR);
	auto result_data = FlatVector::GetData<int8_t>(result);

	info.context.init_m = true;

	while (result_size < args.size()) {
		vector<std::bitset<LANE_LIMIT>> seen(input_size);
		vector<std::bitset<LANE_LIMIT>> visit(input_size);
		vector<std::bitset<LANE_LIMIT>> visit_next(input_size);

		//! mapping of src_value ->  (bfs_num/lane, vector of indices in src_data)
		unordered_map<int64_t, pair<int16_t, vector<int64_t>>> lane_map;
		unordered_map<int16_t, vector<int8_t>> depth_map;
		int8_t bfs_depth = 0;
		auto curr_batch_size = InitialiseBfs(result_size, args.size(), src_data, vdata_src.sel, vdata_src.validity,
		                                     seen, visit, visit_next, lane_map, bfs_depth, depth_map, input_size);
		bool exit_early = false;

		while (!exit_early) {
			bfs_depth++;
			exit_early = true;
			exit_early = BfsWithoutArray(exit_early, id, input_size, info.context, seen, visit, visit_next, bfs_depth,
			                             depth_map);

			visit = visit_next;
			for (auto i = 0; i < input_size; i++) {
				visit_next[i] = 0;
			}
		}

		for (const auto &iter : lane_map) {
			auto value = iter.first;
			auto bfs_num = iter.second.first;
			auto pos = iter.second.second;
			for (auto index : pos) {
				auto target_index = vdata_target.sel->get_index(index);
				result_data[index] = depth_map[bfs_num][target_data[target_index]];
//				if (seen[target_data[target_index]][bfs_num] && seen[value][bfs_num]) {
//					// if(is_bit_set(seen[target_data[index]], bfs_num) & is_bit_set(seen[value], bfs_num) ) {
//					result_data[index] = true;
//				} else {
//					result_data[index] = false;
//				}
			}
		}
		result_size = result_size + curr_batch_size;
	}
}

static unique_ptr<FunctionData> PathLengthBind(ClientContext &context, ScalarFunction &bound_function,
                                               vector<unique_ptr<Expression>> &arguments) {
	return make_unique<PathLengthBindData>(context);
}

void PathLengthFun::RegisterFunction(BuiltinFunctions &set) {
	// params ->id, is_variant, v_size, source, target
	set.AddFunction(ScalarFunction(
	    "path_length",
	    {LogicalType::INTEGER, LogicalType::BOOLEAN, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::TINYINT, PathLengthFunction, false, PathLengthBind));
}

} // namespace duckdb