//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/property_graph_table.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/common.hpp"
#include "duckdb/parser/tableref/basetableref.hpp"
// #include "duckdb/common/enums/statement_type.hpp"
// #include "duckdb/common/exception.hpp"
// #include "duckdb/common/printer.hpp"

namespace duckdb {
//! SQLStatement is the base class of any type of SQL statement.
class PropertyGraphTable {
public:
	PropertyGraphTable() {
	}

	PropertyGraphTable(string name, vector<string> keys, vector<string> labels, bool is_vertex_table) {
	}

	PropertyGraphTable(string name, vector<string> keys, vector<string> labels, bool is_vertex_table,
	                   vector<string> source_key, string source_key_reference, vector<string> destination_key,
	                   string destination_key_reference) {
	}
	//! The name of edge/vertex table
	string name;
	// unique_ptr<BaseTableRef> table;
	// struct boundpropertygraph -> call theat in SQL during match,--> be more resilient
	vector<string> keys;
	//! The labels associated with the table
	vector<string> labels;
	//! The statement length within the query string
	bool is_vertex_table;

	vector<string> source_key;

	string source_key_reference;

	vector<string> destination_key;

	string destination_key_reference;

	bool contains_discriminator;

	string discriminator;

	// string query;

	virtual ~PropertyGraphTable() {
	}

public:
	//! Create a copy of this PropertyGraphStatement
	unique_ptr<PropertyGraphTable> Copy();
	// virtual unique_ptr<PropertyGraphTable> Copy() const = 0;
	void Serialize(Serializer &serializer) const;
	static unique_ptr<PropertyGraphTable> Deserialize(Deserializer &source);
};
} // namespace duckdb
