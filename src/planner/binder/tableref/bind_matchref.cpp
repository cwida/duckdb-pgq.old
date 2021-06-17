
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

// void simple_join(string vertex, string edge) {
//     int x = 0;
//     x+1;
// }

// unordered_set<TableRef> table_set;
// returns table names
static unique_ptr<BaseTableRef> TransformFromTable(string alias, string table_name) {
	auto result = make_unique<BaseTableRef>();
	// TableReferenceType::BASE_TABLE:
	result->alias = alias;
	result->table_name = table_name;
	// result->condition = ;
	return result;
}

// need to populate from_tables and ParsedExpression
// string join_match(vector<string> vertex_tables, vector<string> edge_tables, vector<string> vertex_aliases,
// vector<string> edge_aliases, string table_name) {
//     // auto edge_copy =
//     string result = "";
//     string vertex_alias = vertex_aliases.back();
//     string edge_alias = edge_aliases.back();

//     for (auto vertex_key: vertex_tables) {
//         for (auto edge_key: edge_tables) {
//             // simple_join(vertex_key, edge_key);
//             auto table = TransformFromTable(vertex_alias, table_name, vertex_key, edge_key);
//             // table_set.insert(join_ref);
//             // result += vertex_key + edge_key;
//             // column_name = string(reinterpret_cast<PGValue *>(col_node)->val.str);
// 				// auto colref = make_unique<ColumnRefExpression>(column_name, table_name);
//         }
//     }
//     return result;
// }

unique_ptr<PropertyGraphTable> Binder::FindLabel(PropertyGraphCatalogEntry *pg_table, string &label_name) {

	auto entry = pg_table->label_map_1.find(label_name);

	if (entry == pg_table->label_map_1.end()) {
		throw BinderException("Label %s does not exist in property graph table %s", label_name, pg_table->name);
	}
	// auto property_table = move(entry->second);
	return move(entry->second);
}

// unique_ptr<ParsedExpression> Binder::AndExpression(vector<unique_ptr<ParsedExpression>> conditions) {
//     unique_ptr<ParsedExpression> and_expression;
//     for( auto &condition: conditions) {
//         if(and_expression){
//             and_expression = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND,
//             move(and_expression), move(condition));
//             // where_expression = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND,
//             where_expression, condition);
//         }
//         else{
//             and_expression = move(condition);
//         }
//     }
//     return move(and_expression);
// }

unique_ptr<ParsedExpression> Binder::CreateExpression(vector<string> &vertex_columns, vector<string> &edge_columns,
                                                      string &vertex_table_name, string &edge_table_name) {
	vector<unique_ptr<ParsedExpression>> conditions;
	// = make_unique<ConstantExpression>(Value::INTEGER((int32_t)0));

	// for( auto &condition: conditions) {
	// if(where_expression){
	//     where_expression = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND,
	//     move(where_expression), move(condition));
	//     // where_expression = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND, where_expression,
	//     condition);
	// }
	// else{
	//     where_expression = move(condition);
	// }
	if (vertex_columns.size() != edge_columns.size()) {
		throw BinderException("Vertex columns and edge colums size mismatch.");
	}
	// auto columns_pair = std::make_pair(vertex_columns, edge_columns);
	// for (auto &vertex_column: vertex_columns) {
	//     for(auto &edge_column: edge_columns) {
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
			// where_expression = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND, where_expression,
			// condition);
		} else {
			and_expression = move(condition);
		}
	}
	return and_expression;
	// return AndExpression(conditions);
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
	vector<unique_ptr<ParsedExpression>> conditions;

	auto select_node = make_unique<SelectNode>();
	auto subquery = make_unique<SelectStatement>();
	// auto result = make_unique<JoinRef>();
	// result->type = JoinType::INNER;
	// import error
	// auto result = make_unique<BoundSelectNode>();
	if (ref.param_list.size() < 1) {
		throw BinderException("Match clause needs to contain at least 1 element. ");
	} else if (ref.param_list.size() % 2 == 0) {
		throw BinderException("Match clause missing element. ");
	}
	auto previous_vertex_pattern = move(ref.param_list[0]);
	auto previous_vertex_entry = FindLabel(pg_table, previous_vertex_pattern->label_name);

	from_tables.insert(TransformFromTable(previous_vertex_pattern->alias_name, previous_vertex_entry->name));

	for (idx_t i = 1; i < ref.param_list.size(); i = i + 2) {
		auto edge_pattern = move(ref.param_list[i]);
		auto vertex_pattern = move(ref.param_list[i + 1]);
		// What if odd elements ? eXception ?

		auto edge_entry = FindLabel(pg_table, edge_pattern->label_name);
		// pg_table->label_map_1.find(edge_pattern->label_name);
		auto vertex_entry = FindLabel(pg_table, vertex_pattern->label_name);

		from_tables.insert(TransformFromTable(edge_pattern->alias_name, edge_entry->name));
		from_tables.insert(TransformFromTable(vertex_pattern->alias_name, vertex_entry->name));
		switch (edge_pattern->direction) {
		case MatchDirection::LEFT: {
			// for (auto &vertex_columns: previous_vertex_entry->key) {
			//     for(auto &edge_columns: edge_entry->destination_key) {
			//         auto vertex_colref = make_unique<ColumnRefExpression>(vertex_columns,
			//         previous_vertex_entry->second.name); auto edge_colref =
			//         make_unique<ColumnRefExpression>(edge_columns, edge_entry->second.name);
			//         conditions.push_back(make_unique<ComparisonExpression>(ExpressionType::COMPARE_EQUAL,
			//         vertex_colref, edge_colref));
			//     }
			// }
			auto src_expr = CreateExpression(vertex_entry->keys, edge_entry->source_key, previous_vertex_entry->name,
			                                 edge_entry->name);
			auto dst_expr = CreateExpression(previous_vertex_entry->keys, edge_entry->destination_key,
			                                 previous_vertex_entry->name, edge_entry->name);
			conditions.push_back(move(src_expr));
			conditions.push_back(move(dst_expr));
			// v1.insert(v1.end(),v2.begin(),v2.end());
			// conditions.insert(conditions.end(), src_expr.begin(), src_expr.end());
			// conditions.insert(conditions.end(), dst_expr.begin(), dst_expr.end());
			previous_vertex_entry = move(vertex_entry);
			// auto colref = make_unique<ColumnRefExpression>(column_name, table_name);
			break;
		}
		case MatchDirection::RIGHT: {
			auto src_expr = CreateExpression(previous_vertex_entry->keys, edge_entry->source_key,
			                                 previous_vertex_entry->name, edge_entry->name);
			auto dst_expr = CreateExpression(vertex_entry->keys, edge_entry->destination_key,
			                                 previous_vertex_entry->name, edge_entry->name);
			// conditions.insert(conditions.end(), src_expr.begin(), src_expr.end());
			// conditions.insert(conditions.end(), dst_expr.begin(), dst_expr.end());
			conditions.push_back(move(src_expr));
			conditions.push_back(move(dst_expr));

			// conditions.push_back();
			// conditions.push_back();
			previous_vertex_entry = move(vertex_entry);
			break;
		}
		case MatchDirection::ANY: {
			auto src_expr_1 = CreateExpression(previous_vertex_entry->keys, edge_entry->destination_key,
			                                   previous_vertex_entry->name, edge_entry->name);
			auto dst_expr_1 = CreateExpression(vertex_entry->keys, edge_entry->source_key, previous_vertex_entry->name,
			                                   edge_entry->name);
			auto left_expr =
			    make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND, move(src_expr_1), move(dst_expr_1));

			auto src_expr_2 = CreateExpression(previous_vertex_entry->keys, edge_entry->source_key,
			                                   previous_vertex_entry->name, edge_entry->name);
			auto dst_expr_2 = CreateExpression(vertex_entry->keys, edge_entry->destination_key,
			                                   previous_vertex_entry->name, edge_entry->name);
			auto right_expr =
			    make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND, move(src_expr_2), move(dst_expr_2));

			vector<unique_ptr<ParsedExpression>> temp_vector;
			auto exp =
			    make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_OR, move(left_expr), move(right_expr));
			// temp_vector.push_back(move(exp));
			conditions.push_back(move(exp));
			// conditions.insert(conditions.end(), temp_vector.begin(), temp_vector.end());
			break;
		}
		default:
			throw BinderException("Unknown Edge Pattern direction ");
			break;
		}

		// auto &graph_element_pattern = ref.param_list[i];
		// auto label_name = graph_element_pattern->label_name;
		// auto entry = pg_table->label_map_1.find(label_name);
		// if (entry == pg_table->label_map_1.end()) {
		//     throw BinderException("Label %s does not exist in property graph table %s",
		//     graph_element_pattern->label_name, pg_table->name);
		// }
		// // auto label_entry = pg_table->label_map.find(curr_label);
		// else {

		//     auto edge_table =
		//     auto table_name = entry->name;
		//     // graph_element_pattern->direction;
		//     auto vertex_edge_table = entry->second;

		//     if(graph_element_pattern->is_vertex_pattern) {
		//         if(edge_columns.size() > 0) {
		//             while (edge_columns.size() > 0) {
		//                 auto edge_column = edge_columns.back();
		//                 join_match(vertex_columns, edge_column, vertex_aliases, edge_aliases, table_name);
		//                 edge_columns.pop_back();
		//             }

		//             vertex_columns = vertex_edge_table.keys;
		//             vertex_aliases.push_back(graph_element_pattern->variable_name);
		//         }
		//     }
		//     else {
		//         edge_aliases.push_back(graph_element_pattern->variable_name);
		//         switch (graph_element_pattern->direction)
		//         {
		//         case MatchDirection::LEFT:
		//             join_match(vertex_columns, entry->second.destination_key, vertex_aliases, edge_aliases,
		//             table_name); edge_columns.push_back(entry->second.source_key); break;
		//         case MatchDirection::RIGHT:
		//             join_match(vertex_columns, entry->second.source_key, vertex_aliases, edge_aliases, table_name);
		//             edge_columns.push_back(entry->second.destination_key);
		//             break;
		//         case MatchDirection::ANY:
		//             join_match(vertex_columns, entry->second.destination_key, vertex_aliases, edge_aliases,
		//             table_name); join_match(vertex_columns, entry->second.source_key, vertex_aliases, edge_aliases,
		//             table_name); edge_columns.push_back(entry->second.destination_key);
		//             edge_columns.push_back(entry->second.source_key);
		//             break;
		//         default:
		//             throw BinderException("Unknown Edge Pattern direction ");
		//             break;
		//         }

		// }
		// }
	}
	// iterate through all the transformed table list and Bind.
	unique_ptr<TableRef> cur_root;
	// vector<unique_ptr<BaseTableRef>> from_tables_vector(from_tables.begin(), from_tables.end());

	for (auto &table : from_tables) {
		unique_ptr<BaseTableRef> tmp;
		tmp->alias = table->alias;
		tmp->table_name = table->table_name;

		if (!cur_root) {
			cur_root = move(tmp);
		} else {
			auto new_root = make_unique<CrossProductRef>();
			new_root.get()->left = move(cur_root);
			new_root.get()->right = move(tmp);
			cur_root = move(new_root);
		}
	}
#if 0
    //iterate through all the transformed table list and Bind.
    // result->from_table = Bind(*statement.from_table);
    auto cross_ref = make_unique<CrossProductRef>();
    // import error
    CrossProductRef *cur_root = cross_ref.get();
    for (auto &column : from_columns ) {
    //     // auto n = reinterpret_cast<PGNode *>(node->data.ptr_value);
    //   nique_ptr<TableRef> next = TransformTableRefNode(n);
        auto test = make_unique<BaseTableRef>();

        if (!cur_root->left) {
            cur_root->left = column;
        } else if (!cur_root->right) {
            // auto right = column;
            cur_root->right = column;
        } else {
            auto old_res = move(cross_ref);
            cross_ref = make_unique<CrossProductRef>();
            cross_ref->left = move(old_res);
            cross_ref->right = move(column);
            cur_root = cross_ref.get();
        }
    }
#endif
	select_node->from_table = move(cur_root);
	select_node->select_list = move(ref.columns);

	unique_ptr<ParsedExpression> where_expression;
	// = make_unique<ConstantExpression>(Value::INTEGER((int32_t)0));

	for (auto &condition : conditions) {
		if (where_expression) {
			where_expression = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND,
			                                                      move(where_expression), move(condition));
			// where_expression = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND, where_expression,
			// condition);
		} else {
			where_expression = move(condition);
		}
	}
	if (where_expression) {
		select_node->where_clause = move(where_expression);
	}
	// TransformExpressionList(stmt->targetList, result->select_list);
	// for(auto &node : )
	// return move(result);

	// for (auto &condition : extra_conditions) {
	// 	if (ref.condition) {
	// 		ref.condition = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND, move(ref.condition),
	// 		                                                   move(condition));
	// 	} else {
	// 		ref.condition = move(condition);
	// 	}
	// }

	// ConjunctionExpression(ExpressionType type, vector<unique_ptr<ParsedExpression>> children);

	// auto conjunction_epr = ConjunctionExpression(ExpressionType::COMPARE_EQUAL, vector<unique_ptr<ParsedExpression>>
	// children); BindExpression(conjunction_expr, 0); auto conditions =
	// make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND, move(ref.condition),
	//    move(condition));

	// fix imports
	//    return make_unique<BoundSubqueryRef>(move(result), move(subquery));
	//    auto select = make_unique<SelectStatement>();
	// select->node = GetQueryNode();
	// better way to do this ?? should CTE be here ?

	auto binder = make_unique<Binder>(context, this);
	// BindNode(result);
	subquery->node = move(select_node);
	auto result = make_unique<SubqueryRef>(move(subquery));

	// Stuck at building TableRef
	// SubqueryRef subquery_ref(unique_ptr_cast<SQLStatement, SelectStatement>(subquery->Copy()));
	// BindNode(subquery->node);
	// auto bound_subquery = make_unique<BoundSubqueryRef>(move(binder), move(subquery));
	// Bind(move(result));
	// return move(bound_subquery);
	// auto bound_child = Bind(result.get());
	// return move(bound_subquery);
	// return make_unique<SubqueryRef>(move(select));
	return make_unique<BoundMatchRef>();
}

} // namespace duckdb
