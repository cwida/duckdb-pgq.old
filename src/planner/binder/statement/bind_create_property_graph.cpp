// #include "duckdb/planner/binder.hpp"
// #include "duckdb/planner/constraints/list.hpp"
// #include "duckdb/planner/expression/bound_constant_expression.hpp"
// #include "duckdb/planner/expression_binder/check_binder.hpp"
// #include "duckdb/planner/expression_binder/constant_binder.hpp"
// #include "duckdb/parser/parsed_data/create_table_info.hpp"
// #include "duckdb/planner/parsed_data/bound_create_table_info.hpp"

// unique_ptr<BoundCreatePropertyGraphInfo> Binder::BindCreateTableInfo(unique_ptr<CreateInfo> info) {
// 	auto &base = (CreateMacroInfo &)info;

// 	if (base.function->expression->HasParameter()) {
// 		throw BinderException("Parameter expressions within macro's are not supported!");
// 	}

// 	// create macro binding in order to bind the function
// 	vector<LogicalType> dummy_types;
// 	vector<string> dummy_names;
// 	// positional parameters
// 	for (idx_t i = 0; i < base.function->parameters.size(); i++) {
// 		auto param = (ColumnRefExpression &)*base.function->parameters[i];
// 		if (!param.table_name.empty()) {
// 			throw BinderException("Invalid parameter name '%s'", param.ToString());
// 		}
// 		dummy_types.push_back(LogicalType::SQLNULL);
// 		dummy_names.push_back(param.column_name);
// 	}
// 	// default parameters
// 	for (auto it = base.function->default_parameters.begin(); it != base.function->default_parameters.end(); it++) {
// 		auto &val = (ConstantExpression &)*it->second;
// 		dummy_types.push_back(val.value.type());
// 		dummy_names.push_back(it->first);
// 	}
// 	auto this_macro_binding = make_unique<MacroBinding>(dummy_types, dummy_names, base.name);
// 	macro_binding = this_macro_binding.get();

// 	// create a copy of the expression because we do not want to alter the original
// 	auto expression = base.function->expression->Copy();

// 	// bind it to verify the function was defined correctly
// 	string error;
// 	auto sel_node = make_unique<BoundSelectNode>();
// 	auto group_info = make_unique<BoundGroupInformation>();
// 	SelectBinder binder(*this, context, *sel_node, *group_info);
// 	error = binder.Bind(&expression, 0, false);

// 	if (!error.empty()) {
// 		throw BinderException(error);
// 	}

// 	return BindSchema(info);
// }