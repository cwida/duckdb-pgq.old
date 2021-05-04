#include "duckdb/parser/tableref/matchref.hpp"
#include "duckdb/parser/graph_element_pattern.hpp"
// #include "duckdb/parser/tableref/emptytableref.hpp"
#include "duckdb/parser/transformer.hpp"
#include "duckdb/common/to_string.hpp"
// #include "duckdb/parser/expression/columnref_expression.hpp"

namespace duckdb {

using namespace duckdb_libpgquery;

unique_ptr<GraphElementPattern> Transformer::TransformElementPattern(PGGraphElementPattern *element_pattern) {
	// auto pattern_clause = TransformGraphVariablePattern(element_pattern->pattern_clause);

	auto is_vertex_pattern = element_pattern->is_vertex_pattern;
	MatchDirection direction;
	switch (element_pattern->direction) {
		// auto direction;
	case PG_MATCH_DIR_LEFT:
		direction = MatchDirection::LEFT;
		break;
	case PG_MATCH_DIR_RIGHT:
		direction = MatchDirection::RIGHT;
		break;
	case PG_MATCH_DIR_ANY:
		direction = MatchDirection::ANY;
		break;
	default:
		throw NotImplementedException("Element pattern direction %d not implemented yet", element_pattern->direction);
	}

	auto graph_variable = reinterpret_cast<PGGraphVariablePattern *>(element_pattern->pattern_clause);
	if (!graph_variable->variable_name) {
		if (element_pattern->is_vertex_pattern) {
			string graph_element_name = "__v__" + to_string(vertex_id++);
			return make_unique<GraphElementPattern>(graph_element_name, graph_variable->label_name, is_vertex_pattern);
		} else {
			string graph_element_name = "__e__" + to_string(edge_id++);
			return make_unique<GraphElementPattern>(graph_element_name, graph_variable->label_name, is_vertex_pattern,
			                                        direction);
		}
	}

	return make_unique<GraphElementPattern>(graph_variable->variable_name, graph_variable->label_name,
	                                        is_vertex_pattern, direction);
}

unique_ptr<TableRef> Transformer::TransformMatch(PGMatchPattern *root) {
	auto result = make_unique<MatchRef>();
	auto qname = TransformQualifiedName(root->name);

	result->name = qname.name;
	result->pg_name = string(root->pg_name);

	for (auto node = root->pattern->head; node != nullptr; node = node->next) {
		auto element_pattern = reinterpret_cast<PGGraphElementPattern *>(node->data.ptr_value);
		auto transformed_pattern = TransformElementPattern(element_pattern);
		result->param_list.push_back(move(transformed_pattern));
	}

	TransformExpressionList(root->columns, result->columns);
	// for (auto node = root->columns->head; node != nullptr; node = node->next) {
	//     auto column = reinterpret_cast<PGRangeVar *>(node->data.ptr_value);
	//     auto qname = TransformQualifiedName(column);
	//     auto expression = make_unique<ColumnRefExpression>(qname.name);
	//     // expression->alias = ;
	//     // auto colref = make_unique<ColumnRefExpression>(column_name, table_name);
	//     result->columns.push_back(move(expression));
	// }

	return move(result);
}

} // namespace duckdb
