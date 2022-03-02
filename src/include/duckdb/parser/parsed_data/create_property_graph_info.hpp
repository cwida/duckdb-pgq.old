//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/parsed_data/create_property_graph_info.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/parser/parsed_data/create_info.hpp"
#include "duckdb/parser/property_graph_table.hpp"
#include "duckdb/parser/tableref/basetableref.hpp"
#include "duckdb/parser/property_graph_table.hpp"
#include "duckdb/common/unordered_map.hpp"

namespace duckdb {

struct CreatePropertyGraphInfo : public CreateInfo {
	CreatePropertyGraphInfo() : CreateInfo(CatalogType::PROPERTY_GRAPH_ENTRY) {
	}

	CreatePropertyGraphInfo(string schema, string pg_name)
	    : CreateInfo(CatalogType::PROPERTY_GRAPH_ENTRY, schema), name(pg_name) {
	}

	//! Property Graph name
	string name;
	//! List of vertex tabels
	vector<unique_ptr<PropertyGraphTable>> vertex_tables;
	//! List of edge tabels
	vector<unique_ptr<PropertyGraphTable>> edge_tables;
	//! Labels to table name map : used in match
	unordered_map<string, string> label_map;

	//!
	unordered_map<string, PropertyGraphTable *> label_map_1;

public:
	unique_ptr<CreateInfo> Copy() const override {
		auto result = make_unique<CreatePropertyGraphInfo>(schema, name);
		CopyProperties(*result);
		for (auto &vertex_table : vertex_tables) {
			result->vertex_tables.push_back(vertex_table->Copy());
		}
		for (auto &edge_table : edge_tables) {
			result->edge_tables.push_back(edge_table->Copy());
		}
		return move(result);
	}
};

} // namespace duckdb