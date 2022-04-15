#include "duckdb/function/scalar/sql_pgq_functions.hpp"
#include "duckdb/main/client_context.hpp"

#include <iostream>
#include <algorithm>

namespace duckdb {

struct CheapestPathBindData : public FunctionData {
	ClientContext &context;
	string file_name;

	CheapestPathBindData(ClientContext &context, string &file_name) : context(context), file_name(file_name) {
	}

	unique_ptr<FunctionData> Copy() override {
		return make_unique<CheapestPathBindData>(context, file_name);
	}
};

static int16_t InitialiseBellmanFord(ClientContext &context, const DataChunk &args, int64_t input_size,
                                     const VectorData &vdata_src, const int64_t *src_data, idx_t result_size,
                                     unordered_map<int64_t, vector<int64_t>> &modified,
                                     unordered_map<int64_t, vector<int64_t>> &dists) {
	for (int64_t i = 0; i < input_size; i++) {
		//! Whatever is in v[i] is the offset to the start of the edge indexes. Not the vertex id itself.
		//! auto offset = (int64_t)context.csr_list[id]->v[i];
		modified[i] = std::vector<int64_t>(args.size(), false);
		dists[i] = std::vector<int64_t>(args.size(), INT64_MAX);
	}

	int16_t lanes = 0;
	int16_t curr_batch_size = 0;
	for (idx_t i = result_size; i < args.size() && lanes < LANE_LIMIT; i++) {
		auto src_index = vdata_src.sel->get_index(i);
		if (vdata_src.validity.RowIsValid(src_index)) {
			const int64_t &src_entry = src_data[src_index];
			modified[src_entry][i] = true;
			dists[src_entry][i] = 0;
			curr_batch_size++;
		}
	}
	return curr_batch_size;
}

static void CheapestPathFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CheapestPathBindData &)*func_expr.bind_info;

	int32_t id = args.data[0].GetValue(0).GetValue<int32_t>();
	int64_t input_size = args.data[1].GetValue(0).GetValue<int64_t>();

	auto &src = args.data[2];

	VectorData vdata_src, vdata_target;
	src.Orrify(args.size(), vdata_src);

	auto src_data = (int64_t *)vdata_src.data;

	auto &target = args.data[3];
	target.Orrify(args.size(), vdata_target);
	auto target_data = (int64_t *)vdata_target.data;

	idx_t result_size = 0;
	result.SetVectorType(VectorType::FLAT_VECTOR);
	auto result_data = FlatVector::GetData<int64_t>(result);
	auto &result_validity = FlatVector::Validity(result);

	info.context.init_m = true;
	unordered_map<int64_t, vector<int64_t>> modified;
	unordered_map<int64_t, vector<int64_t>> dists;


	while (result_size < args.size()) {
		int16_t curr_batch_size = InitialiseBellmanFord(info.context, args, input_size, vdata_src, src_data, result_size, modified, dists);
		bool changed = true;
		while (changed) {
			changed = false;
			//! For every v in the graph
			for (int64_t v = 0; v < input_size; v++) {
				//! not modified[v].empty()
				if (!std::all_of(modified[v].begin(), modified[v].end(), [](bool v) { return !v; })) {
					//! Loop through all the neighbours of v
					for (auto index = (int64_t)info.context.csr_list[id]->v_weight[v]; index < (int64_t)info.context.csr_list[id]->v_weight[v + 1];
					     index++) {
						auto n = info.context.csr_list[id]->e[index];
						auto weight = info.context.csr_list[id]->w[index];
						for (uint64_t i = 0; i < modified[v].size(); i++) {
							if (modified[v][i]) {
								auto new_dist = std::min(dists[n][i], dists[v][i] + weight);
								if (new_dist != dists[n][i]) {
									dists[n][i] = new_dist;
									modified[n][i] = true;
									changed = true;
								}
							}
						}
					}
				}
			}
		}
		for (idx_t i = 0; i < args.size(); i++) {
			auto source_index = vdata_src.sel->get_index(i);
			auto target_index = vdata_target.sel->get_index(i);

			if (!vdata_src.validity.RowIsValid(source_index) || !vdata_target.validity.RowIsValid(target_index)) {
				result_validity.SetInvalid(i);
			}

//			const auto &src_entry = src_data[source_index];
			const auto &target_entry = target_data[target_index];
			auto resulting_distance = dists[target_entry][i];
			if (resulting_distance == INT64_MAX) {
				result_validity.SetInvalid(i);
			} else {
				result_data[i] = resulting_distance;
			}
		}
		result_size += curr_batch_size;
	}

}

static unique_ptr<FunctionData> CheapestPathBind(ClientContext &context, ScalarFunction &bound_function,
                                                 vector<unique_ptr<Expression>> &arguments) {
	string file_name;
	if (arguments.size() == 6) {
		file_name = ExpressionExecutor::EvaluateScalar(*arguments[5]).GetValue<string>();

	} else {
		file_name = "timings-test_cheapest.txt";
	}
	return make_unique<CheapestPathBindData>(context, file_name);
}

void CheapestPathFun::RegisterFunction(BuiltinFunctions &set) {
	//! params -> id, v_size, source, target, weight
	set.AddFunction(ScalarFunction(
	    "cheapest_path", {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::BIGINT, CheapestPathFunction, false, CheapestPathBind));
}
} // namespace duckdb