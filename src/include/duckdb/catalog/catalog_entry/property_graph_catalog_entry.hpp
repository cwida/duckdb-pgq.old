//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/catalog/catalog_entry/property_graph_catalog_entry.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/catalog/standard_entry.hpp"
#include "duckdb/common/unordered_map.hpp"
#include "duckdb/parser/property_graph_table.hpp"
#include "duckdb/planner/expression.hpp"

namespace duckdb {

struct CreatePropertyGraphInfo;

//! A table catalog entry
class PropertyGraphCatalogEntry : public StandardEntry {
public:
	//! Create a real PropertyGraphCatalogEntry and initialize storage for it
	PropertyGraphCatalogEntry(Catalog *catalog, SchemaCatalogEntry *schema, CreatePropertyGraphInfo *info);

	string name;
	vector<unique_ptr<PropertyGraphTable>> vertex_tables;
	vector<unique_ptr<PropertyGraphTable>> edge_tables;
	// unordered_map<string, string> label_map;
	unordered_map<string, PropertyGraphTable *> label_map_1;
	vector<LogicalType> types;

public:
	// ! Serialize the meta information of the PropertyGraphCatalogEntry a serializer
	virtual void Serialize(Serializer &serializer);
	//! Deserializes to a CreatePropertyGraphInfo
	static unique_ptr<CreatePropertyGraphInfo> Deserialize(Deserializer &source);

	unique_ptr<CatalogEntry> Copy(ClientContext &context) override;

private:
	void Initialize(CreatePropertyGraphInfo *info);
};

} // namespace duckdb
