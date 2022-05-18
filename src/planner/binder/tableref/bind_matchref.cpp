
#include "duckdb/planner/binder.hpp"
#include "duckdb/parser/tableref/matchref.hpp"
#include "duckdb/parser/tableref.hpp"
#include "duckdb/planner/tableref/bound_matchref.hpp"
#include "duckdb/parser/tableref/crossproductref.hpp"
#include "duckdb/planner/bound_tableref.hpp"
#include "duckdb/parser/tableref/joinref.hpp"
#include "duckdb/parser/tableref/subqueryref.hpp"

#include "duckdb/catalog/catalog_entry/property_graph_catalog_entry.hpp"

#include "duckdb/parser/query_node/select_node.hpp"
#include "duckdb/parser/statement/select_statement.hpp"
#include "duckdb/common/constants.hpp"

#include "duckdb/parser/expression/conjunction_expression.hpp"
#include "duckdb/parser/expression/comparison_expression.hpp"
#include "duckdb/parser/expression/columnref_expression.hpp"
#include "duckdb/parser/expression/function_expression.hpp"
#include "duckdb/parser/expression/subquery_expression.hpp"
#include "duckdb/parser/expression/constant_expression.hpp"
#include "duckdb/parser/expression/cast_expression.hpp"
// #include "duckdb/planner/expression_binder.hpp"
#include "duckdb/parser/expression_map.hpp"
// #include "duckdb/parser/parsed_expression.hpp"

namespace duckdb {

struct GroupingExpressionMap {
	expression_map_t<idx_t> map;
};

static unique_ptr<BaseTableRef> TransformFromTable(const string &alias, const string &table_name) {
	auto result = make_unique<BaseTableRef>();
	if (!alias.empty()) {
		result->alias = alias;
	}
	result->table_name = table_name;
	return result;
}

static GroupingSet VectorToGroupingSet(vector<idx_t> &indexes) {
	GroupingSet result;
	for (idx_t i = 0; i < indexes.size(); i++) {
		result.insert(indexes[i]);
	}
	return result;
}

static void AddGroupByExpression(unique_ptr<ParsedExpression> expression, GroupingExpressionMap &map,
                                 GroupByNode &result, vector<idx_t> &result_set) {

	auto entry = map.map.find(expression.get());
	idx_t result_idx;
	if (entry == map.map.end()) {
		result_idx = result.group_expressions.size();
		map.map[expression.get()] = result_idx;
		result.group_expressions.push_back(move(expression));
	} else {
		result_idx = entry->second;
	}
	result_set.push_back(result_idx);
}

/*
Returning the table name with the associated label. If label does not exist then throw an exception.
*/
PropertyGraphTable *Binder::GetPropertyGraphEntry(PropertyGraphCatalogEntry *pg_table, string &label_name) {

	auto entry = pg_table->label_map_1.find(label_name);

	if (entry == pg_table->label_map_1.end()) {
		throw BinderException("Label %s does not exist in property graph table %s", label_name, pg_table->name);
	}
	// auto property_table = move(entry->second);
	return entry->second;
}

/*
Create the AND expression for the conditions that will be used in the WHERE clause.
*/
unique_ptr<ParsedExpression> Binder::CreateExpression(vector<string> &vertex_columns, vector<string> &edge_columns,
                                                      string &vertex_table_name, string &edge_table_name) {
	vector<unique_ptr<ParsedExpression>> conditions;

	if (vertex_columns.size() != edge_columns.size()) {
		throw BinderException("Vertex columns and edge colums size mismatch.");
	}
	for (idx_t i = 0; i < vertex_columns.size(); i++) {
		auto vertex_colref = make_unique<ColumnRefExpression>(vertex_columns[i], vertex_table_name);
		auto edge_colref = make_unique<ColumnRefExpression>(edge_columns[i], edge_table_name);
		conditions.push_back(
		    make_unique<ComparisonExpression>(ExpressionType::COMPARE_EQUAL, move(vertex_colref), move(edge_colref)));
	}
	unique_ptr<ParsedExpression> and_expression;
	for (auto &condition : conditions) {
		if (and_expression) {
			and_expression = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND, move(and_expression),
			                                                    move(condition));
		} else {
			and_expression = move(condition);
		}
	}
	return and_expression;
}

/*
Verify alias usage across edge patterns. If the alias has not been seen before, then we add it to the map. An alias can
only be repeated if it is used for the same table. Furthermore, a vertex alias cannot be used as an edge alias.
*/
bool Binder::CheckAliasUsage(unordered_map<string, std::tuple<string, bool, string>> &alias_table_map, string &alias,
                             string &table_name, bool is_vertex_pattern, string &label) {
	auto entry = alias_table_map.find(alias);
	if (entry == alias_table_map.end()) {
		alias_table_map[alias] = std::make_tuple(table_name, is_vertex_pattern, label);
		return true;
	} else if (std::get<0>(entry->second) != table_name) {
		throw BinderException("Alias %s used for different tables", alias);
	} else if (std::get<1>(entry->second) != is_vertex_pattern) {
		throw BinderException("Alias %s used for both vertex table and edge table", alias);
	} else if (std::get<0>(entry->second) == table_name && std::get<1>(entry->second) == is_vertex_pattern) {
		return true;
	}
	return false;
}

/*
Returns the label of an alias if it has already been seen before.
Exception is thrown if alias does not have an associated label
*/
static string GetLabel(unordered_map<string, std::tuple<string, bool, string>> &alias_table_map, string &alias) {
	auto entry = alias_table_map.find(alias);
	if (entry == alias_table_map.end()) {
		throw BinderException("Alias %s does not have a label and label has not been defined before.", alias);
	} else {
		return std::get<2>(entry->second);
	}
}

//! max c.rowid() + 1 instead to handle deletes
/*
Helper function to get the max vertex count for csr array allocation.
*/
static unique_ptr<SelectStatement> GetCountTable(PropertyGraphTable *vertex_entry) {
	auto select_count = make_unique<SelectStatement>();
	auto select_inner = make_unique<SelectNode>();
	auto ref = make_unique<BaseTableRef>();
	ref->schema_name = DEFAULT_SCHEMA;
	ref->table_name = vertex_entry->name;

	select_inner->from_table = move(ref);
	auto colref = make_unique<ColumnRefExpression>("rowid", vertex_entry->name);
	vector<unique_ptr<ParsedExpression>> children;
	children.push_back(move(colref));
	auto max_function = make_unique<FunctionExpression>("max", move(children));
	auto size_constant = make_unique<ConstantExpression>(Value::INTEGER((int32_t)1));

	vector<unique_ptr<ParsedExpression>> plus_children;
	plus_children.push_back(move(max_function));
	plus_children.push_back(move(size_constant));
	auto plus_function = make_unique<FunctionExpression>("+", move(plus_children));
	plus_function->is_operator = true;
	select_inner->select_list.push_back(move(plus_function));
	select_count->node = move(select_inner);
	return select_count;
}

static unique_ptr<JoinRef> GetJoinRef(PropertyGraphTable *vertex_entry, PropertyGraphTable *edge_entry) {
	auto first_join_ref = make_unique<JoinRef>();
	first_join_ref->type = JoinType::INNER;
	auto second_join_ref = make_unique<JoinRef>();
	second_join_ref->type = JoinType::INNER;
	auto edge_base_ref = make_unique<BaseTableRef>();
	
	edge_base_ref->table_name = edge_entry->name;
	auto src_base_ref = make_unique<BaseTableRef>();
	src_base_ref->table_name = vertex_entry->name;
	src_base_ref->alias = "src";
	second_join_ref->left = move(edge_base_ref);
	second_join_ref->right = move(src_base_ref);
	auto t_from_ref = make_unique<ColumnRefExpression>(edge_entry->source_key[0], edge_entry->name);
	auto src_cid_ref = make_unique<ColumnRefExpression>(vertex_entry->keys[0], "src");
	second_join_ref->condition =
	    make_unique<ComparisonExpression>(ExpressionType::COMPARE_EQUAL, move(t_from_ref), move(src_cid_ref));
	auto dst_base_ref = make_unique<BaseTableRef>();
	dst_base_ref->table_name = vertex_entry->name;
	dst_base_ref->alias = "dst";
	first_join_ref->left = move(second_join_ref);
	first_join_ref->right = move(dst_base_ref);

	auto t_to_ref = make_unique<ColumnRefExpression>(edge_entry->destination_key[0], edge_entry->name);
	auto dst_cid_ref = make_unique<ColumnRefExpression>(vertex_entry->keys[0], "dst");

	first_join_ref->condition =
	    make_unique<ComparisonExpression>(ExpressionType::COMPARE_EQUAL, move(t_to_ref), move(dst_cid_ref));
	return first_join_ref;
}

static unique_ptr<FunctionExpression> CreateCSRVertexFunction(PropertyGraphTable *vertex_entry) {
	auto vertex_id_constant = make_unique<ConstantExpression>(Value::INTEGER((int32_t)0));
	auto cast_inner_select = move(GetCountTable(vertex_entry));
	auto sub_dense_colref = make_unique<ColumnRefExpression>("dense_id", "sub");
	auto sub_cnt_colref = make_unique<ColumnRefExpression>("cnt", "sub");
	auto count_subquery_expr = make_unique<SubqueryExpression>();
	count_subquery_expr->subquery_type = SubqueryType::SCALAR;
	count_subquery_expr->subquery = move(cast_inner_select);
	vector<unique_ptr<ParsedExpression>> csr_vertex_children;
	csr_vertex_children.push_back(move(vertex_id_constant));
	csr_vertex_children.push_back(move(count_subquery_expr));
	csr_vertex_children.push_back(move(sub_dense_colref));
	csr_vertex_children.push_back(move(sub_cnt_colref));
	return make_unique<FunctionExpression>("create_csr_vertex", move(csr_vertex_children));
}

static unique_ptr<FunctionExpression> CreateSumFunction(PropertyGraphTable *vertex_entry) {
	auto create_vertex_function = CreateCSRVertexFunction(vertex_entry);
	vector<unique_ptr<ParsedExpression>> sum_children;
	sum_children.push_back(move(create_vertex_function));
	return make_unique<FunctionExpression>("sum", move(sum_children));
}

static unique_ptr<SelectStatement> CreateInnerSelectStatement(PropertyGraphTable *vertex_entry,
                                                        PropertyGraphTable *edge_entry) {
	auto inner_select_statment = make_unique<SelectStatement>();
	auto inner_select_node = make_unique<SelectNode>();
	auto c_rowid_colref = make_unique<ColumnRefExpression>("rowid", vertex_entry->name);
	c_rowid_colref->alias = "dense_id";

	auto t_fromid_colref = make_unique<ColumnRefExpression>(edge_entry->source_key[0], edge_entry->name); // t label
	vector<unique_ptr<ParsedExpression>> inner_count_children;
	inner_count_children.push_back(move(t_fromid_colref));
	auto inner_count_function = make_unique<FunctionExpression>("count", move(inner_count_children));
	inner_count_function->alias = "cnt";
	inner_select_node->select_list.push_back(move(c_rowid_colref));
	inner_select_node->select_list.push_back(move(inner_count_function));
	auto c_rowid_colref_1 = make_unique<ColumnRefExpression>("rowid", vertex_entry->name); // c label
	
	GroupByNode gnode;
	vector<idx_t> indexes;
	GroupingExpressionMap map;

	AddGroupByExpression(move(c_rowid_colref_1), map, inner_select_node->groups, indexes);
	inner_select_node->groups.grouping_sets.push_back(VectorToGroupingSet(indexes));
	auto inner_join_ref = make_unique<JoinRef>();
	inner_join_ref->type = JoinType::LEFT;
	auto left_base_ref = make_unique<BaseTableRef>();
	left_base_ref->table_name = vertex_entry->name;
	auto right_base_ref = make_unique<BaseTableRef>();
	right_base_ref->table_name = edge_entry->name;
	inner_join_ref->left = move(left_base_ref);
	inner_join_ref->right = move(right_base_ref);
	// quals for ON
	auto t_join_colref = make_unique<ColumnRefExpression>(edge_entry->source_key[0], edge_entry->name); // t label
	auto c_join_colref = make_unique<ColumnRefExpression>(vertex_entry->keys[0], vertex_entry->name);               // c label
	inner_join_ref->condition =
	    make_unique<ComparisonExpression>(ExpressionType::COMPARE_EQUAL, move(t_join_colref), move(c_join_colref));
	
	inner_select_node->from_table = move(inner_join_ref);
	inner_select_statment->node = move(inner_select_node);
	return inner_select_statment;
}

static unique_ptr<CastExpression> CreateCastExpression(PropertyGraphTable *vertex_entry,
                                                        PropertyGraphTable *edge_entry) {
	auto cast_subquery_expr = make_unique<SubqueryExpression>();
	auto cast_select_node = make_unique<SelectNode>();

	auto inner_select_statement = CreateInnerSelectStatement(vertex_entry, edge_entry);
	auto inner_from_subquery = make_unique<SubqueryRef>(move(inner_select_statement), "sub");
	
	cast_select_node->from_table = move(inner_from_subquery);
	auto sum_function = CreateSumFunction(vertex_entry);
	cast_select_node->select_list.push_back(move(sum_function));
	auto cast_select_stmt = make_unique<SelectStatement>();
	cast_select_stmt->node = move(cast_select_node);
	cast_subquery_expr->subquery = move(cast_select_stmt);
	cast_subquery_expr->subquery_type = SubqueryType::SCALAR;

	return make_unique<CastExpression>(LogicalType::BIGINT, move(cast_subquery_expr));
}

static unique_ptr<FunctionExpression> CreateCsrEdgeFunction(PropertyGraphTable *vertex_entry,
                                                            PropertyGraphTable *edge_entry) {

	// will be another variable in client context
	auto edge_id_constant = make_unique<ConstantExpression>(Value::INTEGER((int32_t)0));
	auto subquery_expr = make_unique<SubqueryExpression>();

	subquery_expr->subquery = move(GetCountTable(vertex_entry));
	subquery_expr->subquery_type = SubqueryType::SCALAR;

	auto cast_expression = CreateCastExpression(vertex_entry, edge_entry);

	auto src_rowid_colref = make_unique<ColumnRefExpression>("rowid", "src");
	auto dst_rowid_colref = make_unique<ColumnRefExpression>("rowid", "dst");

	vector<unique_ptr<ParsedExpression>> csr_edge_children;
	csr_edge_children.push_back(move(edge_id_constant));
	csr_edge_children.push_back(move(subquery_expr));
	csr_edge_children.push_back(move(cast_expression));
	csr_edge_children.push_back(move(src_rowid_colref));
	csr_edge_children.push_back(move(dst_rowid_colref));

	return make_unique<FunctionExpression>("create_csr_edge", move(csr_edge_children));
}

static unique_ptr<FunctionExpression> CreateReachabilityFunction() {
	vector<unique_ptr<ParsedExpression>> reachability_children;
	auto cte_where_src_row = make_unique<ColumnRefExpression>("rowid", "src");
	auto cte_where_dst_row = make_unique<ColumnRefExpression>("rowid", "dst");
	auto cte_vcount = make_unique<ColumnRefExpression>("vcount", "cte1");
	
	auto reachability_id_constant = make_unique<ConstantExpression>(Value::INTEGER((int32_t)0));
	auto reachability_is_variant = make_unique<ConstantExpression>(Value::BOOLEAN(false));
	reachability_children.push_back(move(reachability_id_constant));
	reachability_children.push_back(move(reachability_is_variant));
	reachability_children.push_back(move(cte_vcount));
	reachability_children.push_back(move(cte_where_src_row));
	reachability_children.push_back(move(cte_where_dst_row));

	return make_unique<FunctionExpression>("reachability", move(reachability_children));
}

static unique_ptr<SelectStatement> CreateOuterSelectStatement(PropertyGraphTable *vertex_entry,
                                                              PropertyGraphTable *edge_entry) {

	auto outer_select_statment = make_unique<SelectStatement>();

	auto outer_select_node = make_unique<SelectNode>();

	auto create_csr_edge_function = CreateCsrEdgeFunction(vertex_entry, edge_entry);

	vector<unique_ptr<ParsedExpression>> min_children;
	min_children.push_back(move(create_csr_edge_function));

	auto min_function = make_unique<FunctionExpression>("min", move(min_children));
	min_function->alias = "temp";
	auto vcount_subquery = make_unique<SubqueryExpression>();
	vcount_subquery->subquery_type = SubqueryType::SCALAR;

	vcount_subquery->subquery = move(GetCountTable(vertex_entry));
	vcount_subquery->alias = "vcount";
	outer_select_node->select_list.push_back(move(min_function));
	outer_select_node->select_list.push_back(move(vcount_subquery));
	outer_select_node->from_table = move(GetJoinRef(vertex_entry, edge_entry));

	outer_select_statment->node = move(outer_select_node);
	return outer_select_statment;
}

static unique_ptr<SelectStatement> CreateCTESelectStatement(PropertyGraphTable *previous_vertex,
                                                            PropertyGraphTable *edge_entry,
                                                            PropertyGraphTable *vertex_entry) {

	auto info = make_unique<CommonTableExpressionInfo>();
	auto cte_select_statement = make_unique<SelectStatement>();

	if (previous_vertex->name != vertex_entry->name) {
		throw BinderException("We currently only support CSR creation for the same vertex tables.");
	}
	info->query = move(CreateOuterSelectStatement(vertex_entry, edge_entry));

	auto cte_select_node = make_unique<SelectNode>();
	// TODO: handle multiple
	cte_select_node->cte_map["cte1"] = move(info);

	auto src_cid_col_ref = make_unique<ColumnRefExpression>(vertex_entry->keys[0], "src");
	src_cid_col_ref->alias = "c1id";
	auto dst_cid_col_ref = make_unique<ColumnRefExpression>(vertex_entry->keys[0], "dst");
	dst_cid_col_ref->alias = "c2id";
	auto cte_col_ref = make_unique<ColumnRefExpression>("temp", "cte1");
	cte_col_ref->alias = "csr";
	cte_select_node->select_list.push_back(move(src_cid_col_ref));
	cte_select_node->select_list.push_back(move(dst_cid_col_ref));
	
	auto cte_ref = make_unique<BaseTableRef>();
	cte_ref->schema_name = DEFAULT_SCHEMA;
	cte_ref->table_name = "cte1";
	auto outer_cross_ref = make_unique<CrossProductRef>();
	outer_cross_ref->left = move(cte_ref);
	auto src_base_ref = make_unique<BaseTableRef>();
	src_base_ref->table_name = vertex_entry->name;
	src_base_ref->alias = "src";
	outer_cross_ref->right = move(src_base_ref);

	auto dst_base_ref = make_unique<BaseTableRef>();
	dst_base_ref->table_name = vertex_entry->name;
	dst_base_ref->alias = "dst";
	auto cross_ref = make_unique<CrossProductRef>();
	cross_ref->left = move(outer_cross_ref);
	cross_ref->right = move(dst_base_ref);
	cte_select_node->from_table = move(cross_ref);

	vector<unique_ptr<ParsedExpression>> cte_conditions;
	auto reachability_function = CreateReachabilityFunction();
	auto cte_where_expression = make_unique<ComparisonExpression>(ExpressionType::COMPARE_EQUAL,
	                                                              move(reachability_function), move(cte_col_ref));

	cte_select_node->where_clause = move(cte_where_expression);
	cte_select_statement->node = move(cte_select_node);
	return cte_select_statement;
}


unique_ptr<BoundTableRef> Binder::Bind(MatchRef &ref) {
	auto pg_table =
	    Catalog::GetCatalog(context).GetEntry<PropertyGraphCatalogEntry>(context, DEFAULT_SCHEMA, ref.pg_name, true);
	if (!pg_table) {
		throw BinderException("Property Graph Table %s does not exist.", ref.pg_name);
	}
	vector<string> vertex_columns;
	vector<string> vertex_aliases;
	vector<vector<string>> edge_columns;
	vector<string> edge_aliases;
	unordered_set<unique_ptr<BaseTableRef>> from_tables;
	// mapping alias to tuple (table name, is_vertex_table, label)
	unordered_map<string, std::tuple<string, bool, string>> alias_table_map;
	vector<unique_ptr<ParsedExpression>> conditions;

	auto select_node = make_unique<SelectNode>();
	auto subquery = make_unique<SelectStatement>();
	bool flag = false;
	auto cte_select_statement = make_unique<SelectStatement>();

	if (ref.param_list.empty()) {
		throw BinderException("Match clause needs to contain at least 1 element. ");
	} else if (ref.param_list.size() % 2 == 0) {
		throw BinderException("Match clause missing element as size of pattern is even.");
	}
	auto previous_vertex_pattern = move(ref.param_list[0]);

	auto previous_vertex_entry = GetPropertyGraphEntry(pg_table, previous_vertex_pattern->label_name);
	auto table = TransformFromTable(previous_vertex_pattern->alias_name, previous_vertex_entry->name);

	alias_table_map[previous_vertex_pattern->alias_name] = std::make_tuple(
	    previous_vertex_entry->name, previous_vertex_pattern->is_vertex_pattern, previous_vertex_pattern->label_name);
	from_tables.insert(move(table));

	for (idx_t i = 1; i < ref.param_list.size(); i = i + 2) {
		auto edge_pattern = move(ref.param_list[i]);
		auto vertex_pattern = move(ref.param_list[i + 1]);

		auto edge_label = (!edge_pattern->label_name.empty()) ? edge_pattern->label_name
		                                                      : GetLabel(alias_table_map, edge_pattern->alias_name);
		auto vertex_label = (!vertex_pattern->label_name.empty())
		                        ? vertex_pattern->label_name
		                        : GetLabel(alias_table_map, vertex_pattern->alias_name);

		auto edge_entry = GetPropertyGraphEntry(pg_table, edge_label);
		auto vertex_entry = GetPropertyGraphEntry(pg_table, vertex_label);

		auto edge_alias_usage = CheckAliasUsage(alias_table_map, edge_pattern->alias_name, edge_entry->name,
		                                        edge_pattern->is_vertex_pattern, edge_label);
		auto vertex_alias_usage = CheckAliasUsage(alias_table_map, vertex_pattern->alias_name, vertex_entry->name,
		                                          vertex_pattern->is_vertex_pattern, vertex_label);

		if (edge_alias_usage && vertex_alias_usage) {

			switch (edge_pattern->star_pattern) {
			case MatchStarPattern::ALL: {
				flag = true;

				cte_select_statement =
				    CreateCTESelectStatement(previous_vertex_entry, edge_entry, vertex_entry);
				//! Insert into from_tables and call continue with an alias. check if this can be use with BaseTableRef.
				// outer_subquery_exp->subquery = move(outer_select_statment);
				break;
			}
			case MatchStarPattern::BOUNDED: {
			}
			case MatchStarPattern::NONE: {
				// do nothing;
			}
			}

			from_tables.insert(TransformFromTable(edge_pattern->alias_name, edge_entry->name));
			from_tables.insert(TransformFromTable(vertex_pattern->alias_name, vertex_entry->name));

			switch (edge_pattern->direction) {
			case MatchDirection::LEFT: {
				auto src_expr = CreateExpression(vertex_entry->keys, edge_entry->source_key, vertex_pattern->alias_name,
				                                 edge_pattern->alias_name);
				auto dst_expr = CreateExpression(previous_vertex_entry->keys, edge_entry->destination_key,
				                                 previous_vertex_pattern->alias_name, edge_pattern->alias_name);
				conditions.push_back(move(src_expr));
				conditions.push_back(move(dst_expr));
				previous_vertex_entry = vertex_entry;
				break;
			}
			case MatchDirection::RIGHT: {
				auto src_expr = CreateExpression(previous_vertex_entry->keys, edge_entry->source_key,
				                                 previous_vertex_pattern->alias_name, edge_pattern->alias_name);
				auto dst_expr = CreateExpression(vertex_entry->keys, edge_entry->destination_key,
				                                 vertex_pattern->alias_name, edge_pattern->alias_name);

				conditions.push_back(move(src_expr));
				conditions.push_back(move(dst_expr));
				previous_vertex_entry = vertex_entry;
				break;
			}
			case MatchDirection::ANY: {
				auto src_left_expr = CreateExpression(previous_vertex_entry->keys, edge_entry->destination_key,
				                                      previous_vertex_pattern->alias_name, edge_pattern->alias_name);
				auto dst_left_expr = CreateExpression(vertex_entry->keys, edge_entry->source_key,
				                                      vertex_pattern->alias_name, edge_pattern->alias_name);
				auto combined_left_expr = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND,
				                                                             move(src_left_expr), move(dst_left_expr));

				auto src_right_expr = CreateExpression(previous_vertex_entry->keys, edge_entry->source_key,
				                                       previous_vertex_pattern->alias_name, edge_pattern->alias_name);
				auto dst_right_expr = CreateExpression(vertex_entry->keys, edge_entry->destination_key,
				                                       vertex_pattern->alias_name, edge_pattern->alias_name);
				auto combined_right_expr = make_unique<ConjunctionExpression>(
				    ExpressionType::CONJUNCTION_AND, move(src_right_expr), move(dst_right_expr));

				auto exp = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_OR, move(combined_left_expr),
				                                              move(combined_right_expr));
				conditions.push_back(move(exp));
				break;
			}
			default:
				throw BinderException("Unknown Edge Pattern direction ");
				break;
			}
		} else {
			throw BinderException("Incorrect alias used %s %s", edge_pattern->alias_name, vertex_pattern->alias_name);
		}
	}
	unique_ptr<TableRef> cur_root;

	if (flag) {
		auto result = make_unique<SubqueryRef>(move(cte_select_statement), ref.name);
		return Bind(*result);
	}
	for (auto &it : alias_table_map) {
		auto tmp = TransformFromTable(it.first, std::get<0>(it.second));

		if (!cur_root) {
			cur_root = move(tmp);
		} else {
			auto new_root = make_unique<CrossProductRef>();
			new_root.get()->left = move(cur_root);
			new_root.get()->right = move(tmp);
			cur_root = move(new_root);
		}
	}
	select_node->from_table = move(cur_root);
	select_node->select_list = move(ref.columns);

	unique_ptr<ParsedExpression> where_expression;

	for (auto &condition : conditions) {
		if (where_expression) {
			where_expression = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND,
			                                                      move(where_expression), move(condition));
		} else {
			where_expression = move(condition);
		}
	}
	if (where_expression) {
		select_node->where_clause = move(where_expression);
	}

	subquery->node = move(select_node);
	auto result = make_unique<SubqueryRef>(move(subquery), ref.name);
	// result->alias = ref.name;

	return Bind(*result);
}

} // namespace duckdb
