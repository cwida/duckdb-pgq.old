//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/tableref/matchref.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/parser/tableref.hpp"
// #include "duckdb/parser/parsed_expression.hpp"
#include "duckdb/parser/graph_element_pattern.hpp"
// #include "duckdb/common/enums/tableref_type.hpp"
// include "duckdb/common/enums/join_type.hpp"
// #include "duckdb/common/unordered_set.hpp"
#include "duckdb/parser/parsed_expression.hpp"
#include "duckdb/common/vector.hpp"

namespace duckdb {

class MatchRef : public TableRef {
public:
	MatchRef() : TableRef(TableReferenceType::MATCH) {
	}

	string name;
	string schema;

	string pg_name;
	vector<unique_ptr<GraphElementPattern>> param_list;
	// vector<string> columns;
	vector<unique_ptr<ParsedExpression>> columns;
	// look if you can use ColumnDefinition
	// vector<ColumnDefinition> columns;
	unique_ptr<ParsedExpression> where_clause;
	vector<string> path_names;

public:
	// bool Equals(const TableRef *other_) const override;

	unique_ptr<TableRef> Copy() override;

	//! Serializes a blob into a DummyTableRef
	void Serialize(Serializer &serializer) override;
	//! Deserializes a blob back into a DummyTableRef
	static unique_ptr<TableRef> Deserialize(Deserializer &source);
};
} // namespace duckdb
