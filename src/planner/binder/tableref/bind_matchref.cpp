
#include "duckdb/planner/binder.hpp"
#include "duckdb/parser/tableref/matchref.hpp"
#include "duckdb/parser/tableref.hpp"
#include "duckdb/planner/tableref/bound_matchref.hpp"
#include "duckdb/parser/tableref/crossproductref.hpp"
#include "duckdb/planner/bound_tableref.hpp"

#include "duckdb/catalog/catalog_entry/property_graph_catalog_entry.hpp"
// #include "duckdb/parser/graph_element_pattern.hpp"
// #include "duckdb/parser/tableref/joinref.hpp"
// #include "duckdb/common/unordered_set.hpp"
// #include "duckdb/common/vector.hpp"
// #include "duckdb/planner/tableref/bound_subqueryref.hpp"

#include "duckdb/parser/query_node/select_node.hpp"
#include "duckdb/parser/statement/select_statement.hpp"
#include "duckdb/common/constants.hpp"

#include "duckdb/parser/expression/conjunction_expression.hpp"
#include "duckdb/parser/expression/comparison_expression.hpp"
#include "duckdb/parser/expression/columnref_expression.hpp"
// #include "duckdb/planner/expression_binder.hpp"
// #include "duckdb/parser/parsed_expression.hpp"

namespace duckdb {

static unique_ptr<BaseTableRef> TransformFromTable(string alias, string table_name) {
	auto result = make_unique<BaseTableRef>();
	result->alias = alias;
	result->table_name = table_name;
	// RVO exception --> look more ??
	return result;
}

PropertyGraphTable *Binder::FindLabel(PropertyGraphCatalogEntry *pg_table, string &label_name) {

	auto entry = pg_table->label_map_1.find(label_name);

	if (entry == pg_table->label_map_1.end()) {
		throw BinderException("Label %s does not exist in property graph table %s", label_name, pg_table->name);
	}
	// auto property_table = move(entry->second);
	return entry->second;
}

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

static string GetLabel(unordered_map<string, std::tuple<string, bool, string>> &alias_table_map, string &alias) {
	auto entry = alias_table_map.find(alias);
	if (entry == alias_table_map.end()) {
		throw BinderException("Alias %s does not have a label and label has not been defined before.", alias);
	} else {
		return std::get<2>(entry->second);
	}
}

unique_ptr<BoundTableRef> Binder::Bind(MatchRef &ref) {
	auto pg_table =
	    Catalog::GetCatalog(context).GetEntry<PropertyGraphCatalogEntry>(context, DEFAULT_SCHEMA, ref.pg_name, true);
	if (!pg_table) {
		throw BinderException("Property Graph Table %s does not exist.", ref.pg_name);
	}
	// pg_table->label_map;
	vector<string> vertex_columns;
	vector<string> vertex_aliases;
	vector<vector<string>> edge_columns;
	vector<string> edge_aliases;
	unordered_set<unique_ptr<BaseTableRef>> from_tables;
	unordered_map<string, std::tuple<string, bool, string>> alias_table_map;
	vector<unique_ptr<ParsedExpression>> conditions;

	auto select_node = make_unique<SelectNode>();
	auto subquery = make_unique<SelectStatement>();

	if (ref.param_list.size() < 1) {
		throw BinderException("Match clause needs to contain at least 1 element. ");
	} else if (ref.param_list.size() % 2 == 0) {
		throw BinderException("Match clause missing element as size of pattern is even.");
	}
	auto previous_vertex_pattern = move(ref.param_list[0]);

	// if(!previous_vertex_pattern->label_name){
	// 	throw BinderException("First vertex label cannot be empty");
	// }
	auto previous_vertex_entry = FindLabel(pg_table, previous_vertex_pattern->label_name);
	auto table = TransformFromTable(previous_vertex_pattern->alias_name, previous_vertex_entry->name);

	alias_table_map[previous_vertex_pattern->alias_name] = std::make_tuple(
	    previous_vertex_entry->name, previous_vertex_pattern->is_vertex_pattern, previous_vertex_pattern->label_name);
	// table.get();
	from_tables.insert(move(table));

	for (idx_t i = 1; i < ref.param_list.size(); i = i + 2) {
		auto edge_pattern = move(ref.param_list[i]);
		auto vertex_pattern = move(ref.param_list[i + 1]);

		auto edge_label = (edge_pattern->label_name != "") ? edge_pattern->label_name
		                                                   : GetLabel(alias_table_map, edge_pattern->alias_name);
		auto vertex_label = (vertex_pattern->label_name != "") ? vertex_pattern->label_name
		                                                       : GetLabel(alias_table_map, vertex_pattern->alias_name);

		auto edge_entry = FindLabel(pg_table, edge_label);
		auto vertex_entry = FindLabel(pg_table, vertex_label);

		auto edge_alias_usage = CheckAliasUsage(alias_table_map, edge_pattern->alias_name, edge_entry->name,
		                                        edge_pattern->is_vertex_pattern, edge_label);
		auto vertex_alias_usage = CheckAliasUsage(alias_table_map, vertex_pattern->alias_name, vertex_entry->name,
		                                          vertex_pattern->is_vertex_pattern, vertex_label);

		if (edge_alias_usage && vertex_alias_usage) {

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
				previous_vertex_entry = move(vertex_entry);
				break;
			}
			case MatchDirection::RIGHT: {
				auto src_expr = CreateExpression(previous_vertex_entry->keys, edge_entry->source_key,
				                                 previous_vertex_pattern->alias_name, edge_pattern->alias_name);
				auto dst_expr = CreateExpression(vertex_entry->keys, edge_entry->destination_key,
				                                 vertex_pattern->alias_name, edge_pattern->alias_name);

				conditions.push_back(move(src_expr));
				conditions.push_back(move(dst_expr));
				previous_vertex_entry = move(vertex_entry);
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

	// 	for (auto &table : from_tables) {
	//     auto tmp = make_unique<BaseTableRef>();
	//     tmp->alias = table->alias;
	//     tmp->table_name = table->table_name;

	//     if (!cur_root) {
	//         cur_root = move(tmp);
	//     } else {
	//         auto new_root = make_unique<CrossProductRef>();
	//         new_root.get()->left = move(cur_root);
	//         new_root.get()->right = move(tmp);
	//         cur_root = move(new_root);
	//     }
	// }

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
