#include "duckdb/parser/property_graph_table.hpp"
#include "duckdb/common/serializer.hpp"

namespace duckdb {

unique_ptr<PropertyGraphTable> PropertyGraphTable::Copy() {
	// unique_ptr<PropertyGraphTable> PropertyGraphTable::Copy() const{

	// return make_unique<PropertyGraphTable>(move(table), labels, keys);
	// auto result = make_unique<PropertyGraphTable>();

	// 	//     // CopyProperties(*result);
	// result->name = name;
	// for(auto &label: labels) {
	// 	result->labels.push_back(label);
	// }

	// for(auto &key: keys) {
	// 	result->keys.push_back(key);
	// }
	// result->is_vertex_table = is_vertex_table;

	// if(!is_vertex_table){
	// 	// result->destination_key = destination_key;
	// 	for(auto &dest_key: destination_key) {
	// 		result->destination_key.push_back(dest_key);
	// 	}
	// 	result->destination_key_reference = destination_key_reference;
	// 	for(auto &src_key: source_key) {
	// 		result->source_key.push_back(src_key);
	// 	}
	// 	result->source_key_reference = source_key_reference;
	// }
	if (is_vertex_table) {
		return make_unique<PropertyGraphTable>(name, keys, labels, is_vertex_table);
	} else {
		return make_unique<PropertyGraphTable>(name, keys, labels, is_vertex_table, source_key, source_key_reference,
		                                       destination_key, destination_key_reference);
	}
}

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
