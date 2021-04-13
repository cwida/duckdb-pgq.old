#include "duckdb/parser/property_graph_table.hpp"
#include "duckdb/common/serializer.hpp"

namespace duckdb {

// unique_ptr<PropertyGraphTable> PropertyGraphTable::Copy() const {
// 	// PropertyGraphTable copy();
// 	// other->name = name;
// 	return make_unique<PropertyGraphTable>(move(table), labels, keys);
// }

void PropertyGraphTable::Serialize(Serializer &serializer) const {
	serializer.WriteString(name);
	// serializer.Write<BaseTableRef>(table);
	serializer.WriteStringVector(keys);
	serializer.WriteStringVector(labels);
	// use bool flag instead ?
	serializer.Write<bool>(is_vertex_table);
	if (!is_vertex_table) {
		serializer.WriteStringVector(source_key);
		serializer.WriteString(source_key_reference);
		serializer.WriteStringVector(destination_key);
		serializer.WriteString(destination_key_reference);
	}
}

unique_ptr<PropertyGraphTable> PropertyGraphTable::Deserialize(Deserializer &source) {
	// auto column_name = source.Read<string>();
	// auto column_type = LogicalType::Deserialize(source);
	// auto default_value = source.ReadOptional<ParsedExpression>();
	auto pg_table = make_unique<PropertyGraphTable>();
	pg_table->name = source.Read<string>();
	source.ReadStringVector(pg_table->keys);
	source.ReadStringVector(pg_table->labels);
	pg_table->is_vertex_table = source.Read<bool>();
	if (!pg_table->is_vertex_table) {
		source.ReadStringVector(pg_table->source_key);
		pg_table->source_key_reference = source.Read<string>();
		source.ReadStringVector(pg_table->destination_key);
		pg_table->destination_key_reference = source.Read<string>();
	}
	return pg_table;
}

} // namespace duckdb
