//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/property_graph_table.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/common.hpp"
// #include "duckdb/common/enums/statement_type.hpp"
// #include "duckdb/common/exception.hpp"
// #include "duckdb/common/printer.hpp"

namespace duckdb {
//! SQLStatement is the base class of any type of SQL statement.
class PropertyGraphTable {
public:
	// SQLStatement(StatementType type) : type(type) {};
	// virtual ~SQLStatement() {
	// }
    PropertyGraphTable(){

    }
    PropertyGraphTable(string name, vector<string> keys, vector<string> labels);
	//! The name of edge/vertex table
	string name;
    vector<string> keys;
	//! The labels associated with the table
	vector<string> labels;
	//! The statement length within the query string
	vector<string> source_key;
    string source_key_reference;

    string destination_key_reference;

    vector<string> destination_key;
	// //! The query text that corresponds to this SQL statement
	// string query;

public:
	//! Create a copy of this SelectStatement
	virtual unique_ptr<PropertyGraphTable> Copy() {
        auto result = make_unique<PropertyGraphTable>();
        // CopyProperties(*result);
        result->name = name;
        result->source_key_reference = source_key_reference;
        result->keys = keys;
    }

    
		// CopyProperties(*result);
		// result->index_type = index_type;
		// result->index_name = index_name;
		// result->unique = unique;

};
} // namespace duckdb
