#include "duckdb/function/scalar/sql_pgq_functions.hpp"
#include "duckdb/main/client_context.hpp"

#include <iostream>

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

static void InitialiseBellmanFord(const DataChunk &args, int64_t input_size, const VectorData &vdata_src,
                                  const int64_t *src_data, idx_t result_size,
                                  unordered_map<int64_t, vector<int64_t>> &modified,
                                  unordered_map<int64_t, vector<int64_t>> &dists) {
	int16_t lanes = 0;
	for (idx_t i = result_size; i < args.size() && lanes < LANE_LIMIT; i++) {
		auto src_index = vdata_src.sel->get_index(i);
		if (vdata_src.validity.RowIsValid(src_index)) {
			const int64_t &src_entry = src_data[src_index];
			auto entry = modified.find(src_entry);
			if (entry == modified.end()) {
				modified[src_entry] = std::vector<int64_t>(input_size, false);
				dists[src_entry] = std::vector<int64_t>(input_size, INT64_MAX);
			}
		}
	}
}
static void CheapestPathFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (ShortestPathBindData &)*func_expr.bind_info;

	int32_t id = args.data[0].GetValue(0).GetValue<int32_t>();
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
	unordered_map<int64_t, vector<int64_t>> modified;
	unordered_map<int64_t, vector<int64_t>> dists;

	while (result_size < args.size()) {
		InitialiseBellmanFord(args, input_size, vdata_src, src_data, result_size, modified, dists);
		std::cout << "BREAK POINT" << std::endl;
	}

}


static unique_ptr<FunctionData> CheapestPathBind(ClientContext &context, ScalarFunction &bound_function,
                                                 vector<unique_ptr<Expression>> &arguments) {
	string file_name;
	if (arguments.size() == 6) {
		file_name = ExpressionExecutor::EvaluateScalar(*arguments[5]).GetValue<string>();

	} else {
		file_name = "timings-test.txt";
	}
	return make_unique<MsbfsBindData>(context, file_name);
}

void CheapestPathFun::RegisterFunction(BuiltinFunctions &set) {
	//! params -> id, v_size, source, target, weight
	set.AddFunction(ScalarFunction("cheapest_path",
	                               {LogicalType::INTEGER, LogicalType::BIGINT,
	                                LogicalType::BIGINT, LogicalType::BIGINT},
	                               LogicalType::UBIGINT, CheapestPathFunction, false, CheapestPathBind));
}
} // namespace duckdb