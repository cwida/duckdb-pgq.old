//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/graph_element_pattern.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/common.hpp"

namespace duckdb {

// should we keep NONE ??
enum class MatchDirection : uint8_t { LEFT, RIGHT, ANY, NONE };

enum class MatchDistanceType : uint8_t { SHORTEST, CHEAPEST };

enum class MatchStarPattern : uint8_t { NONE, ALL, BOUNDED };
//! SQLStatement is the base class of any type of SQL statement.
class GraphElementPattern {
public:
	GraphElementPattern() {
	}

	GraphElementPattern(string alias_name, string label_name, bool is_vertex_pattern) {
		this->alias_name = alias_name;
		this->label_name = label_name;
		this->is_vertex_pattern = is_vertex_pattern;
	}

	GraphElementPattern(string alias_name, string label_name, bool is_vertex_pattern, MatchDirection direction,
	                    MatchStarPattern star_pattern) {
		this->alias_name = alias_name;
		this->label_name = label_name;
		this->is_vertex_pattern = is_vertex_pattern;
		this->direction = direction;
		this->star_pattern = star_pattern;
	}

	GraphElementPattern(string alias_name, string label_name, bool is_vertex_pattern, MatchDirection direction,
	                    MatchStarPattern star_pattern, MatchDistanceType distance_type) {
		this->alias_name = alias_name;
		this->label_name = label_name;
		this->is_vertex_pattern = is_vertex_pattern;
		this->direction = direction;
		this->star_pattern = star_pattern;
	}

	GraphElementPattern(string alias_name, string label_name, bool is_vertex_pattern, MatchDirection direction,
	                    MatchStarPattern star_pattern, int lower_bound, int upper_bound) {
		this->alias_name = alias_name;
		this->label_name = label_name;
		this->is_vertex_pattern = is_vertex_pattern;
		this->direction = direction;
		this->star_pattern = star_pattern;
		this->lower_bound = lower_bound;
		this->upper_bound = upper_bound;
	}

	GraphElementPattern(string alias_name, string label_name, bool is_vertex_pattern, MatchDirection direction,
	                    MatchStarPattern star_pattern, MatchDistanceType distance_type, int lower_bound,
	                    int upper_bound) {
		this->alias_name = alias_name;
		this->label_name = label_name;
		this->is_vertex_pattern = is_vertex_pattern;
		this->direction = direction;
		this->star_pattern = star_pattern;
		this->lower_bound = lower_bound;
		this->upper_bound = upper_bound;
	}

	string alias_name;
	string label_name;
	bool is_vertex_pattern;

	MatchDirection direction;
	MatchStarPattern star_pattern;
	int lower_bound;
	int upper_bound;
	virtual ~GraphElementPattern() {
	}

public:
	//! Create a copy of this PropertyGraphStatement
	unique_ptr<GraphElementPattern> Copy();
	// void Serialize(Serializer &serializer) const;
	// static unique_ptr<GraphElementPattern> Deserialize(Deserializer &source);
};
} // namespace duckdb
