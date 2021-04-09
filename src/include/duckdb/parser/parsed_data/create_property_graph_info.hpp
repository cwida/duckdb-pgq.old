//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/parsed_data/create_property_graph_info.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/parser/parsed_data/create_property_graph_info.hpp"
#include "duckdb/parser/parsed_data/create_info.hpp"
#include "duckdb/parser/property_graph_table.hpp"
#include "duckdb/parser/tableref/basetableref.hpp"

namespace duckdb {

struct CreatePropertyGraphInfo : public CreateInfo {
	CreatePropertyGraphInfo() : CreateInfo(CatalogType::PROPERTY_GRAPH_ENTRY) {
	}

	// CreateViewInfo() : CreateInfo(CatalogType::VIEW_ENTRY) {
	// }
	// CreateViewInfo(string schema, string view_name)
	//     : CreateInfo(CatalogType::VIEW_ENTRY, schema), view_name(view_name) {
	// }

	// add second constructor with arguments ???

	// add copy function
	string name;
	vector<unique_ptr<PropertyGraphTable>> vertex_tables;
	vector<unique_ptr<PropertyGraphTable>> edge_tables;
	// vector<unique_ptr<BaseTableRef>> table_ref_list;

public:
	unique_ptr<CreateInfo> Copy() const override {
		auto result = make_unique<CreatePropertyGraphInfo>();
		CopyProperties(*result);
		return move(result);
	}
};

} // namespace duckdb