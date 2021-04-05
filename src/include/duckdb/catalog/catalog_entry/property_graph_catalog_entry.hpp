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
#include "duckdb/parser/constraint.hpp"
#include "duckdb/planner/bound_constraint.hpp"
#include "duckdb/planner/expression.hpp"

namespace duckdb {

// class ColumnStatistics;
class DataTable;
struct CreatePropertyGraphInfo;
// struct BoundCreateTableInfo;

// struct RenameColumnInfo;
// struct AddColumnInfo;
// struct RemoveColumnInfo;
// struct SetDefaultInfo;
// struct ChangeColumnTypeInfo;

//! A table catalog entry
class PropertyGraphCatalogEntry : public StandardEntry {
public:
	//! Create a real PropertyGraphCatalogEntry and initialize storage for it
	// ViewCatalogEntry(Catalog *catalog, SchemaCatalogEntry *schema, CreateViewInfo *info);

	PropertyGraphCatalogEntry(Catalog *catalog, SchemaCatalogEntry *schema, CreatePropertyGraphInfo *info);
	//! A reference to the underlying storage unit used for this table
	// std::shared_ptr<DataTable> storage;

	// vector<PropertyGraphTable> vertex_tables;
	// vector<PropertyGraphTable> edge_tables;
	string name;
	vector<unique_ptr<PropertyGraphTable>> vertex_tables;
	vector<unique_ptr<PropertyGraphTable>> edge_tables;
	vector<unique_ptr<BaseTableRef>> table_ref_list;

public:
	//! Serialize the meta information of the PropertyGraphCatalogEntry a serializer
	virtual void Serialize(Serializer &serializer);
	//! Deserializes to a CreatePropertyGraphInfo
	static unique_ptr<CreatePropertyGraphInfo> Deserialize(Deserializer &source);

	unique_ptr<CatalogEntry> Copy(ClientContext &context) override;

private:
	void Initialize(CreatePropertyGraphInfo *info);
};
} // namespace duckdb
