#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/parser/parsed_data/create_scalar_function_info.hpp"
#include "duckdb/planner/expression/bound_function_expression.hpp"
#include "sqlpgq_functions.hpp"

namespace duckdb {

struct DeleteCsrBindData : public FunctionData {
	ClientContext &context;
	int32_t id;

	DeleteCsrBindData(ClientContext &context, int32_t id) : context(context), id(id) {
	}

	unique_ptr<FunctionData> Copy() override {
		return make_unique<DeleteCsrBindData>(context, id);
	}
};

static unique_ptr<FunctionData> DeleteCsrBind(ClientContext &context, ScalarFunction &bound_function,
                                              vector<unique_ptr<Expression>> &arguments) {

	if (!arguments[0]->IsFoldable()) {
		throw NotImplementedException("CSR ID should be a constant");
	}

	int32_t id = ExpressionExecutor::EvaluateScalar(*arguments[0]).GetValue<int32_t>();
	if (id + 1 > context.csr_list.size()) {
		throw ConstraintException("Invalid ID");
	}
	D_ASSERT(context.csr_list[id]);
//	if (!(context.initialized_v && context.initialized_e && context.initialized_w)) {
//		throw ConstraintException("Need to initialize CSR before doing cheapest path");
//	}
	child_list_t<LogicalType> struct_children;
	struct_children.push_back(make_pair("id", LogicalType::INTEGER));
	bound_function.return_type = LogicalType::STRUCT(move(struct_children));
	return make_unique<DeleteCsrBindData>(context, id);
}

static void DeleteCsrFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (DeleteCsrBindData &)*func_expr.bind_info;
	//# TODO Add error handling
	if (info.id + 1 > info.context.csr_list.size()) {
		throw ConstraintException("Invalid ID");
	}
	info.context.csr_list[info.id].reset();
	info.context.csr_list.erase(info.context.csr_list.begin() + info.id);


	result.SetVectorType(VectorType::CONSTANT_VECTOR);
	auto &child_entries = StructVector::GetEntries(result);
	child_entries[0]->Reference(info.id);
}

CreateScalarFunctionInfo SQLPGQFunctions::GetDeleteCsrFunction() {
	ScalarFunctionSet set("delete_csr_by_id");

	set.AddFunction(
	    ScalarFunction({LogicalType::INTEGER}, LogicalType::ANY, DeleteCsrFunction, false, DeleteCsrBind));

	return CreateScalarFunctionInfo(set);
}
} // namespace duckdb