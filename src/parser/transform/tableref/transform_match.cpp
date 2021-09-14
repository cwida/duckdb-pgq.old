#include "duckdb/parser/tableref/matchref.hpp"
#include "duckdb/parser/graph_element_pattern.hpp"
// #include "duckdb/parser/tableref/emptytableref.hpp"
#include "duckdb/parser/transformer.hpp"
#include "duckdb/common/to_string.hpp"
// #include "duckdb/parser/expression/columnref_expression.hpp"

namespace duckdb {

using namespace duckdb_libpgquery;

unique_ptr<GraphElementPattern> Transformer::TransformElementPattern(PGGraphElementPattern *element_pattern) {

	MatchDirection direction;
	string edge_alias, vertex_alias;
	auto graph_variable = reinterpret_cast<PGGraphVariablePattern *>(element_pattern->pattern_clause);

	auto label_name = graph_variable->label_name ? graph_variable->label_name : "";

	if (element_pattern->is_vertex_pattern) {
		if (!graph_variable->alias_name) {
			vertex_alias = "__v__" + to_string(vertex_id++);
		} else {
			vertex_alias = graph_variable->alias_name;
		}
		// return graph_element_pattern;

		return make_unique<GraphElementPattern>(vertex_alias, label_name, element_pattern->is_vertex_pattern);
	} else {
		switch (element_pattern->direction) {
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
			throw NotImplementedException("Element pattern direction %d not implemented yet",
			                              element_pattern->direction);
		}

		if (!graph_variable->alias_name) {
			edge_alias = "__e__" + to_string(edge_id++);
		} else {
			edge_alias = graph_variable->alias_name;
		}

		switch (element_pattern->star_pattern) {
		case PG_STAR_NONE:
			return make_unique<GraphElementPattern>(edge_alias, label_name, element_pattern->is_vertex_pattern,
			                                        direction, MatchStarPattern::NONE);
		case PG_STAR_ALL:
			return make_unique<GraphElementPattern>(edge_alias, label_name, element_pattern->is_vertex_pattern,
			                                        direction, MatchStarPattern::ALL);
		case PG_STAR_BOUNDED: {
			if (element_pattern->lower_bound < 0 || element_pattern->upper_bound < 0) {
				throw ParserException("Lower and upper bound have to be greater and 0");
			} else if (element_pattern->lower_bound > element_pattern->upper_bound) {
				throw ParserException("Lower bound has to be less than equal to upper bound");
			} else {
				return make_unique<GraphElementPattern>(edge_alias, label_name, element_pattern->is_vertex_pattern,
				                                        direction, MatchStarPattern::BOUNDED,
				                                        element_pattern->lower_bound, element_pattern->upper_bound);
			}
		}
		default:
			throw NotImplementedException("Element pattern direction %d not implemented yet",
			                              element_pattern->direction);
		}
		// return make_unique<GraphElementPattern>(edge_alias, label_name, element_pattern->is_vertex_pattern,
		// direction);
	}
}

unique_ptr<TableRef> Transformer::TransformMatch(PGMatchPattern *root) {
	auto result = make_unique<MatchRef>();
	auto qname = TransformQualifiedName(root->name);

	result->name = qname.name;
	result->pg_name = string(root->pg_name);

	for (auto node = root->pattern->head; node != nullptr; node = node->next) {
		// list of second member
		// root->pattern->head->data.ptr_value
		auto path_list = (PGList *)node->data.ptr_value;
		auto path_name = (PGValue *)path_list->head->data.ptr_value;
		if (path_name)
			result->path_names.push_back(string(path_name->val.str));
		auto pattern_list = (PGList *)path_list->tail->data.ptr_value;
		for (auto node_1 = pattern_list->head; node_1 != nullptr; node_1 = node_1->next) {
			auto element_pattern = reinterpret_cast<PGGraphElementPattern *>(node_1->data.ptr_value);
			auto transformed_pattern = TransformElementPattern(element_pattern);
			result->param_list.push_back(move(transformed_pattern));
		}
	}

	TransformExpressionList(root->columns, result->columns);

	return move(result);
}

} // namespace duckdb
