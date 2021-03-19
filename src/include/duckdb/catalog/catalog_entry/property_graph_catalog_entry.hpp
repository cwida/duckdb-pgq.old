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
	//! Create a real TableCatalogEntry and initialize storage for it
	PropertyGraphCatalogEntry(Catalog *catalog, SchemaCatalogEntry *schema, CreatePropertyGraphInfo *info):
        : StandardEntry(CatalogType::PROPERTY_GRAPH_ENTRY, schema, catalog, info->name));
	                  std::shared_ptr<DataTable> inherited_storage = nullptr);

	//! A reference to the underlying storage unit used for this table
	// std::shared_ptr<DataTable> storage;
	//! A list of columns that are part of this table
	// vector<ColumnDefinition> columns;
	//! A list of constraints that are part of this table
	// vector<unique_ptr<Constraint>> constraints;
	//! A list of constraints that are part of this table
	// vector<unique_ptr<BoundConstraint>> bound_constraints;
	//! A map of column name to column index
	// unordered_map<string, column_t> name_map;

    // vector<PropertyGraphTable> vertex_tables;
	// vector<PropertyGraphTable> edge_tables;

public:
	// unique_ptr<CatalogEntry> AlterEntry(ClientContext &context, AlterInfo *info) override;
	//! Returns whether or not a column with the given name exists
	// bool ColumnExists(const string &name);
	//! Returns a reference to the column of the specified name. Throws an
	//! exception if the column does not exist.
	// ColumnDefinition &GetColumn(const string &name);
	//! Returns a list of types of the table
	// vector<LogicalType> GetTypes();
	// ! Returns a list of types of the specified columns of the table
	// vector<LogicalType> GetTypes(const vector<column_t> &column_ids);
	// string ToSQL() override;

	// ! Add lower case aliases to a name map (e.g. "Hello" -> "hello" is also acceptable)
	// static void AddLowerCaseAliases(unordered_map<string, column_t> &name_map);


	//! Serialize the meta information of the TableCatalogEntry a serializer
	virtual void Serialize(Serializer &serializer);
	//! Deserializes to a CreateTableInfo
	static unique_ptr<CreatePropertyGraphInfo> Deserialize(Deserializer &source);
	

	unique_ptr<CatalogEntry> Copy(ClientContext &context) override;

	// void SetAsRoot() override;

	// void CommitAlter(AlterInfo &info);
	// void CommitDrop();

private:
	void Initialize(CreatePropertyGraphInfo *info);
};
} // namespace duckdb
