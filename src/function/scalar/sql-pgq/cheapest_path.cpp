#include "duckdb/function/scalar/sql_pgq_functions.hpp"
#include "duckdb/main/client_context.hpp"

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
static void CheapestPathFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (ShortestPathBindData &)*func_expr.bind_info;

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
	//! params -> id, is_variant, v_size, source, target, weight
	set.AddFunction(ScalarFunction("cheapest_path",
	                               {LogicalType::INTEGER, LogicalType::BOOLEAN, LogicalType::BIGINT,
	                                LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	                               LogicalType::UBIGINT, CheapestPathFunction, false, CheapestPathBind));
}
} // namespace duckdb