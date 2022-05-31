#include "duckdb/catalog/catalog_entry/property_graph_catalog_entry.hpp"

#include "duckdb/catalog/catalog_entry/schema_catalog_entry.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/serializer.hpp"
// #include "duckdb/parser/parsed_data/alter_table_info.hpp"
#include "duckdb/parser/parsed_data/create_property_graph_info.hpp"
#include "duckdb/common/limits.hpp"

#include <algorithm>

namespace duckdb {

void PropertyGraphCatalogEntry::Initialize(CreatePropertyGraphInfo *info) {
	this->name = info->name;
	this->vertex_tables = move(info->vertex_tables);
	this->edge_tables = move(info->edge_tables);
	this->label_map_1 = move(info->label_map_1);
}

PropertyGraphCatalogEntry::PropertyGraphCatalogEntry(Catalog *catalog, SchemaCatalogEntry *schema,
                                                     CreatePropertyGraphInfo *info)
    : StandardEntry(CatalogType::PROPERTY_GRAPH_ENTRY, schema, catalog, info->name)
// , vertex_tables(info->vertex_tables),edge_tables(info->edge_tables), label_map_1(info->label_map_1)
{
	Initialize(info);
}

void PropertyGraphCatalogEntry::Serialize(Serializer &serializer) {
	D_ASSERT(!internal);
	serializer.WriteString(schema->name);
	serializer.WriteString(name);
	D_ASSERT(vertex_tables.size() <= NumericLimits<uint32_t>::Maximum());
	serializer.Write<uint32_t>((uint32_t)vertex_tables.size());
	for (auto &vertex_table : vertex_tables) {
		vertex_table->Serialize(serializer);
	}
	serializer.Write<uint32_t>((uint32_t)edge_tables.size());
	for (auto &edge_table : edge_tables) {
		edge_table->Serialize(serializer);
	}
}

unique_ptr<CreatePropertyGraphInfo> PropertyGraphCatalogEntry::Deserialize(Deserializer &source) {
	auto info = make_unique<CreatePropertyGraphInfo>();
	info->schema = source.Read<string>();
	auto vertex_table_count = source.Read<uint32_t>();
	for (uint32_t i = 0; i < vertex_table_count; i++) {
		info->vertex_tables.push_back(PropertyGraphTable::Deserialize(source));
	}
	auto edge_table_count = source.Read<uint32_t>();
	for (uint32_t i = 0; i < edge_table_count; i++) {
		info->edge_tables.push_back(PropertyGraphTable::Deserialize(source));
	}
	return info;
}

// string PropertyGraphCatalogEntry::ToSQL() {
// 	if (sql.size() == 0) {
// 		throw NotImplementedException("Cannot convert VIEW to SQL because it was not created with a SQL statement");
// 	}
// 	return sql + "\n;";
// }

unique_ptr<CatalogEntry> PropertyGraphCatalogEntry::Copy(ClientContext &context) {
	D_ASSERT(!internal);
	auto create_info = make_unique<CreatePropertyGraphInfo>();
	// create_info->query = unique_ptr_cast<SQLStatement, SelectStatement>(query->Copy());
	// for (idx_t i = 0; i < edge_tables.size(); i++) {
	// 	create_info->edge_tables.push_back(edge_tables[i]);
	// }
	// for (idx_t i = 0; i < vertex_tables.size(); i++) {
	// 	create_info->vertex_tables.push_back(vertex_tables[i]);
	// }
	create_info->name = name;
	// create_info->temporary = temporary;
	// create_info->sql = sql;

	return make_unique<PropertyGraphCatalogEntry>(catalog, schema, create_info.get());
}

} // namespace duckdb
