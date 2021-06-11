#include "duckdb/parser/tableref/matchref.hpp"
#include "duckdb/planner/binder.hpp"
#include "duckdb/planner/tableref/bound_matchref.hpp"
#include "duckdb/catalog/catalog_entry/property_graph_catalog_entry.hpp"
// #include "duckdb/parser/graph_element_pattern.hpp"
// #include "duckdb/parser/tableref/joinref.hpp"
// #include "duckdb/common/unordered_set.hpp"
// #include "duckdb/planner/tableref/bound_subqueryref.hpp"
// #include "duckdb/parser/tableref/crossproductref.hpp"
// #include "duckdb/parser/expression/conjunction_expression.hpp"
// #include "duckdb/planner/expression_binder.hpp"
// #include "duckdb/parser/parsed_expression.hpp"


namespace duckdb {

// void simple_join(string vertex, string edge) {
//     int x = 0;
//     x+1;
// }

// unordered_set<TableRef> table_set;
//returns table names
unique_ptr<TableRef> TransformFromTable(string alias, string table_name, string vertex_key, string edge_key) {
	auto result = make_unique<BaseTableRef>();
    result->alias = alias;
    result->table_name = table_name;
    // result->condition = ;
	return move(result);
}



string join_match(vector<string> vertex_tables, vector<string> edge_tables, vector<string> vertex_aliases, vector<string> edge_aliases, string table_name) {
    // auto edge_copy =
    string result = "";
    string vertex_alias = vertex_aliases.back();
    string edge_alias = edge_aliases.back();

    for (auto vertex_key: vertex_tables) {
        for (auto edge_key: edge_tables) {
            // simple_join(vertex_key, edge_key);
            auto table = TransformFromTable(vertex_alias, table_name, vertex_key, edge_key);
            // table_set.insert(join_ref);
            // result += vertex_key + edge_key;

        }
    }
    return result;
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
    // unordered_set<TableRef> from_columns;

    // auto result = make_unique<JoinRef>();
    // result->type = JoinType::INNER;
    //import error
    // auto result = make_unique<BoundSelectNode>();
    
    for (idx_t i = 0; i < ref.param_list.size(); i++) {
		auto &graph_element_pattern = ref.param_list[i];
        auto curr_label = graph_element_pattern->label_name;
        auto entry = pg_table->label_map_1.find(curr_label);
		if (entry == pg_table->label_map_1.end()) {
            throw BinderException("Label %s does not exist in property graph table %s", graph_element_pattern->label_name, pg_table->name);
        }
        // auto label_entry = pg_table->label_map.find(curr_label);
        if(entry != pg_table->label_map_1.end()) {
            auto table_name = entry->second.name;
            graph_element_pattern->direction; 
            auto vertex_edge_table = entry->second;
            
            if(graph_element_pattern->is_vertex_pattern) {
                if(edge_columns.size() > 0) {
                    while (edge_columns.size() > 0) {
                        auto edge_column = edge_columns.back();
                        join_match(vertex_columns, edge_column, vertex_aliases, edge_aliases, table_name);
                        edge_columns.pop_back();
                    }
                    
                    vertex_columns = vertex_edge_table.keys;
                    vertex_aliases.push_back(graph_element_pattern->variable_name);
                }
            }
            else {
                edge_aliases.push_back(graph_element_pattern->variable_name);
                switch (graph_element_pattern->direction)
                {
                case MatchDirection::LEFT:
                    join_match(vertex_columns, entry->second.destination_key, vertex_aliases, edge_aliases, table_name);
                    edge_columns.push_back(entry->second.source_key);
                    break;
                case MatchDirection::RIGHT:
                    join_match(vertex_columns, entry->second.source_key, vertex_aliases, edge_aliases, table_name);
                    edge_columns.push_back(entry->second.destination_key);
                    break;
                case MatchDirection::ANY:
                    join_match(vertex_columns, entry->second.destination_key, vertex_aliases, edge_aliases, table_name);
                    join_match(vertex_columns, entry->second.source_key, vertex_aliases, edge_aliases, table_name);
                    edge_columns.push_back(entry->second.destination_key);
                    edge_columns.push_back(entry->second.source_key);
                    break;
                default:
                    throw BinderException("Unknown Edge Pattern direction ");
                    break;
                }
            
            }
        }
    }
    //iterate through all the transformed table list and Bind.
    // result->from_table = Bind(*statement.from_table);
    // auto cross_ref = make_unique<CrossProductRef>();
    // import error
    // CrossProductRef *cur_root = cross_ref.get();
    // for (auto column : from_columns ) {
    //     // auto n = reinterpret_cast<PGNode *>(node->data.ptr_value);
    //     // unique_ptr<TableRef> next = TransformTableRefNode(n);
    //     if (!cur_root->left) {
    //         cur_root->left = move(column);
    //     } else if (!cur_root->right) {
    //         cur_root->right = move(column);
    //     } else {
    //         auto old_res = move(cross_ref);
    //         result = make_unique<CrossProductRef>();
    //         cross_ref->left = move(old_res);
    //         cross_ref->right = move(column);
    //         cur_root = cross_ref.get();
    //     }
    // }
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
    
    // auto conjunction_epr = ConjunctionExpression(ExpressionType::COMPARE_EQUAL, vector<unique_ptr<ParsedExpression>> children);
    // BindExpression(conjunction_expr, 0);
    // auto conditions = make_unique<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND, move(ref.condition),
			                                                //    move(condition));
    
    // fix imports
//    return make_unique<BoundSubqueryRef>(move(result), move(subquery));
//    auto select = make_unique<SelectStatement>();
	// select->node = GetQueryNode();
    //better way to do this ?? should CTE be here ? 
    
    
    
    
    // auto binder = make_unique<Binder>(context, this);
    // auto bound_subquery = make_unique<BoundSubqueryRef>(move(binder), move(result));
	// return move(bound_subquery);
    // return make_unique<SubqueryRef>(move(select));
    return make_unique<BoundMatchRef>();
}

} // namespace duckdb
