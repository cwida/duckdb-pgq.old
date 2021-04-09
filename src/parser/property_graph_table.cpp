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
	serializer.Write<idx_t>(labels.size());
	for (auto &label : labels) {
		serializer.WriteString(label);
	}
	serializer.Write<idx_t>(keys.size());
	for (auto &key : keys) {
		serializer.WriteString(key);
	}
	// type.Serialize(serializer);
	// serializer.WriteOptional(default_value);
}

unique_ptr<PropertyGraphTable> PropertyGraphTable::Deserialize(Deserializer &source) {
	// auto column_name = source.Read<string>();
	// auto column_type = LogicalType::Deserialize(source);
	// auto default_value = source.ReadOptional<ParsedExpression>();
	auto pg_table = make_unique<PropertyGraphTable>();
	pg_table->name = source.Read<string>();
	auto label_size = source.Read<idx_t>();
	for (idx_t i = 0; i < label_size; i++) {
		pg_table->labels.push_back(source.Read<string>());
	}
	auto key_size = source.Read<idx_t>();
	for (idx_t i = 0; i < key_size; i++) {
		pg_table->keys.push_back(source.Read<string>());
	}
	return pg_table;
}

} // namespace duckdb
