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

	GraphElementPattern(string alias_name, string label_name, bool is_vertex_pattern, MatchDirection direction) {
		this->alias_name = alias_name;
		this->label_name = label_name;
		this->is_vertex_pattern = is_vertex_pattern;
		this->direction = direction;
	}

	string alias_name;
	string label_name;
	bool is_vertex_pattern;

	MatchDirection direction;

	virtual ~GraphElementPattern() {
	}

public:
	//! Create a copy of this PropertyGraphStatement
	unique_ptr<GraphElementPattern> Copy();
	// void Serialize(Serializer &serializer) const;
	// static unique_ptr<GraphElementPattern> Deserialize(Deserializer &source);
};
} // namespace duckdb
