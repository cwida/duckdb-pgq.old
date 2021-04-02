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
	// SQLStatement(StatementType type) : type(type) {};
	// virtual ~SQLStatement() {
	// }
    PropertyGraphTable(){

    }

	PropertyGraphTable(vector<string> keys, vector<string> labels);

    // PropertyGraphTable(unique_ptr<BaseTableRef> table, vector<string> keys, vector<string> labels);

	PropertyGraphTable(	string source_key_reference, string destination_key_reference);
	// PropertyGraphTable(unique_ptr<BaseTableRef> table, vector<string> keys, vector<string> labels, vector<string> source_key, 
	// 					string source_key_reference, vector<string> destination_key, string destination_key_reference);
	//! The name of edge/vertex table
	string name;
	unique_ptr<BaseTableRef> table;
    vector<string> keys;
	//! The labels associated with the table
	vector<string> labels;
	//! The statement length within the query string
	vector<string> source_key;

    string source_key_reference;

	vector<string> destination_key;

    string destination_key_reference;
	// //! The query text that corresponds to this SQL statement
	// string query;

public:
	//! Create a copy of this SelectStatement
	virtual unique_ptr<PropertyGraphTable> Copy() {
        auto result = make_unique<PropertyGraphTable>();
		
    //     // CopyProperties(*result);
        result->name = name;
		result->labels = labels;
		// result->table = table;
		result->destination_key = destination_key;
		result->destination_key_reference = destination_key_reference;
        result->source_key = source_key;
		result->source_key_reference = source_key_reference;
        result->keys = keys;
		return move(result);
    }

    
		// CopyProperties(*result);
		// result->index_type = index_type;
		// result->index_name = index_name;
		// result->unique = unique;

};
} // namespace duckdb
