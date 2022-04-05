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

template <typename T, typename S>
std::unordered_map<int16_t, std::vector<S>> createCopy(unordered_map<int16_t, std::vector<T>> const &dict) {
	unordered_map<int16_t, std::vector<S>> new_dict;

	for (auto val : dict) {
		std::vector<S> old_vector = std::vector<S>(val.second.begin(), val.second.end());
		std::replace(old_vector.begin(), old_vector.end(), (S)std::numeric_limits<T>::max(),
		             std::numeric_limits<S>::max());
		new_dict[val.first] = old_vector;
	}
	return new_dict;
}

static int16_t InitialiseBfs(idx_t curr_batch, idx_t size, int64_t *src_data, const SelectionVector *src_sel,
                             const ValidityMask &src_validity, vector<std::bitset<LANE_LIMIT>> &seen,
                             vector<std::bitset<LANE_LIMIT>> &visit, vector<std::bitset<LANE_LIMIT>> &visit_next,
                             unordered_map<int64_t, pair<int16_t, vector<int64_t>>> &lane_map, const uint64_t bfs_depth,
                             unordered_map<int16_t, vector<uint8_t>> &depth_map, int64_t input_size) {
	int16_t lanes = 0;
	int16_t curr_batch_size = 0;

	for (idx_t i = curr_batch; i < size && lanes < LANE_LIMIT; i++) {
		auto src_index = src_sel->get_index(i);

		if (src_validity.RowIsValid(src_index)) {
			int64_t &src_entry = src_data[src_index];
			auto entry = lane_map.find(src_entry);
			if (entry == lane_map.end()) {
				lane_map[src_entry].first = lanes;
				seen[src_entry][lanes] = true;
				visit[src_entry][lanes] = true;
				depth_map[lanes] = vector<uint8_t>(input_size, UINT8_MAX);
				depth_map[lanes][src_entry] = (uint8_t)bfs_depth;
				lanes++;
			}
			lane_map[src_entry].second.push_back(i);
			curr_batch_size++;
		}
	}
	return curr_batch_size;
}

template <typename T>
static bool BfsWithoutArray(bool exit_early, int32_t id, int64_t input_size, ClientContext &context,
                            vector<std::bitset<LANE_LIMIT>> &seen, vector<std::bitset<LANE_LIMIT>> &visit,
                            vector<std::bitset<LANE_LIMIT>> &visit_next, uint64_t bfs_depth,
                            unordered_map<int16_t, vector<T>> &depth_map) {
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
				if (depth_map[map_index][bfs_index] != std::numeric_limits<T>::max()) {
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
	auto start = std::chrono::high_resolution_clock::now();
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
	auto target_data = (uint64_t *)vdata_target.data;

	idx_t result_size = 0;
	result.SetVectorType(VectorType::FLAT_VECTOR);
	auto result_data = FlatVector::GetData<uint64_t>(result);
	auto &validity = FlatVector::Validity(result);

	info.context.init_m = true;

	while (result_size < args.size()) {
		vector<std::bitset<LANE_LIMIT>> seen(input_size);
		vector<std::bitset<LANE_LIMIT>> visit(input_size);
		vector<std::bitset<LANE_LIMIT>> visit_next(input_size);

		//! mapping of src_value ->  (bfs_num/lane, vector of indices in src_data)
		unordered_map<int64_t, pair<int16_t, vector<int64_t>>> lane_map;
		unordered_map<int16_t, vector<uint8_t>> depth_map_uint8;
		unordered_map<int16_t, vector<uint16_t>> depth_map_uint16;
		unordered_map<int16_t, vector<uint32_t>> depth_map_uint32;
		unordered_map<int16_t, vector<uint64_t>> depth_map_uint64;
		uint64_t bfs_depth = 0;
		auto curr_batch_size = InitialiseBfs(result_size, args.size(), src_data, vdata_src.sel, vdata_src.validity,
		                                     seen, visit, visit_next, lane_map, bfs_depth, depth_map_uint8, input_size);
		bool exit_early = false;
		while (!exit_early) {
			if (bfs_depth == UINT8_MAX) {
				depth_map_uint16 = createCopy<uint8_t, uint16_t>(depth_map_uint8);
			} else if (bfs_depth == UINT16_MAX) {
				depth_map_uint32 = createCopy<uint16_t, uint32_t>(depth_map_uint16);
			} else if (bfs_depth == UINT32_MAX) {
				depth_map_uint64 = createCopy<uint32_t, uint64_t>(depth_map_uint32);
			}
			bfs_depth++;
			exit_early = true;
			if (bfs_depth < UINT8_MAX) {
				exit_early = BfsWithoutArray(exit_early, id, input_size, info.context, seen, visit, visit_next, bfs_depth,
				                             depth_map_uint8);
			} else if (bfs_depth >= UINT8_MAX && bfs_depth < UINT16_MAX) {
				exit_early = BfsWithoutArray(exit_early, id, input_size, info.context, seen, visit, visit_next, bfs_depth,
				                             depth_map_uint16);
			} else if (bfs_depth >= UINT16_MAX && bfs_depth < UINT32_MAX) {
				exit_early = BfsWithoutArray(exit_early, id, input_size, info.context, seen, visit, visit_next, bfs_depth,
				                             depth_map_uint32);
			} else if (bfs_depth >= UINT32_MAX && bfs_depth < UINT64_MAX) {
				exit_early = BfsWithoutArray(exit_early, id, input_size, info.context, seen, visit, visit_next, bfs_depth,
				                             depth_map_uint64);
			}


			visit = visit_next;
			for (auto i = 0; i < input_size; i++) {
				visit_next[i] = 0;
			}
		}

		for (const auto &iter : lane_map) {
			auto bfs_num = iter.second.first;
			auto pos = iter.second.second;
			for (auto index : pos) {
				auto target_index = vdata_target.sel->get_index(index);
				uint64_t value = 0;
				if (bfs_depth < UINT8_MAX) {
					value = (uint64_t)depth_map_uint8[bfs_num][target_data[target_index]];
					if (value == UINT8_MAX) {
						validity.SetInvalid(target_index);
					}
				} else if (bfs_depth >= UINT8_MAX && bfs_depth < UINT16_MAX) {
					value = (uint64_t)depth_map_uint16[bfs_num][target_data[target_index]];
					if (value == UINT16_MAX) {
						validity.SetInvalid(target_index);
					}
				} else if (bfs_depth >= UINT16_MAX && bfs_depth < UINT32_MAX) {
					value = (uint64_t)depth_map_uint32[bfs_num][target_data[target_index]];
					if (value == UINT32_MAX) {
						validity.SetInvalid(target_index);
					}
				} else if (bfs_depth >= UINT32_MAX && bfs_depth < UINT64_MAX) {
					value = (uint64_t)depth_map_uint64[bfs_num][target_data[target_index]];
					if (value == UINT64_MAX) {
						validity.SetInvalid(target_index);
					}
				}
				result_data[index] = value;
			}
		}
		result_size = result_size + curr_batch_size;
	}
	auto end = std::chrono::high_resolution_clock::now();

	auto int_s = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "TEST" << std::endl;
	std::cout << "PathLengthFunction() elapsed time is " << int_s.count() << " milliseconds )" << std::endl;
}

static unique_ptr<FunctionData> PathLengthBind(ClientContext &context, ScalarFunction &bound_function,
                                               vector<unique_ptr<Expression>> &arguments) {
	return make_unique<PathLengthBindData>(context);
}

void PathLengthFun::RegisterFunction(BuiltinFunctions &set) {
	// params ->id, is_variant, v_size, source, target
	set.AddFunction(ScalarFunction(
	    "shortest_path",
	    {LogicalType::INTEGER, LogicalType::BOOLEAN, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::UBIGINT, PathLengthFunction, false, PathLengthBind));
}

} // namespace duckdb