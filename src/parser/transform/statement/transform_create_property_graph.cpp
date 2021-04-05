#include "duckdb/parser/statement/create_statement.hpp"
#include "duckdb/parser/transformer.hpp"
#include "duckdb/parser/parsed_data/create_property_graph_info.hpp"
#include "duckdb/parser/tableref/basetableref.hpp"
#include "duckdb/parser/property_graph_table.hpp"

namespace duckdb {

using namespace duckdb_libpgquery;

unique_ptr<PropertyGraphTable> Transformer::TranformPropertyGraphTable(PGPropertyGraphTable *table) {
	// string
	// auto pg_table = make_unique<PropertyGraphTable>();
	// info->name = string(stmt->name);
	vector<string> keys, labels, source_key, destination_key;
	string name, source_key_reference, destination_key_reference;

	auto qname = TransformQualifiedName(table->name);
	auto ref = TransformRangeVar(table->name);
	auto tableref = make_unique<BaseTableRef>();
	tableref->table_name = table->name->relname;
	if (table->name->schemaname) {
		tableref->schema_name = table->name->schemaname;
	}

	for (auto kc = table->keys->head; kc; kc = kc->next) {
		keys.push_back(string(reinterpret_cast<PGValue *>(kc->data.ptr_value)->val.str));
	}

	for (auto kc = table->labels->head; kc; kc = kc->next) {
		labels.push_back(string(reinterpret_cast<PGValue *>(kc->data.ptr_value)->val.str));
	}

	if (table->is_vertex_table) {
		// 	// info->vertex_tables.push_back()
		// 	info->vertex_tables.push_back(PropertyGraphTable(qname.name, keys, labels));
		// return 	make_unique<PropertyGraphTable>(keys, labels);
		return make_unique<PropertyGraphTable>(move(tableref), keys, labels);
		// return unique_ptr<PropertyGraphTable>(new PropertyGraphTable(keys, labels));
	} else {
		for (auto kc = table->source_key->head; kc; kc = kc->next) {
			source_key.push_back(string(reinterpret_cast<PGValue *>(kc->data.ptr_value)->val.str));
		}

		for (auto kc = table->destination_key->head; kc; kc = kc->next) {
			destination_key.push_back(string(reinterpret_cast<PGValue *>(kc->data.ptr_value)->val.str));
		}

		destination_key_reference = string(table->destination_key_reference->relname);
		source_key_reference = string(table->source_key_reference->relname);
		// Find a way to use reference
		return make_unique<PropertyGraphTable>(move(tableref), keys, labels, source_key, source_key_reference,
		                                       destination_key, destination_key_reference);
		// auto pg_table = make_unique<PropertyGraphTable>;
		// pg_table->ref = move(ref);
		// return make_unique<PropertyGraphTable>(source_key_reference, destination_key_reference);
		// return unique_ptr<PropertyGraphTable>(new PropertyGraphTable(source_key_reference,
		// destination_key_reference));
		// look into make_unique. Do we need to modfiy Copy
		// info->edge_tables.push_back(pg_table);
	}
}

unique_ptr<CreateStatement> Transformer::TransformCreatePropertyGraph(PGNode *node) {
	D_ASSERT(node);
	D_ASSERT(node->type == T_PGCreatePropertyGraphStmt);

	auto stmt = reinterpret_cast<PGCreatePropertyGraphStmt *>(node);
	D_ASSERT(stmt);

	auto result = make_unique<CreateStatement>();
	auto info = make_unique<CreatePropertyGraphInfo>();

	info->name = string(stmt->name);
	info->on_conflict = OnCreateConflict::ERROR_ON_CONFLICT;

	for (auto c = stmt->vertex_tables->head; c != NULL; c = lnext(c)) {
		auto node = reinterpret_cast<PGNode *>(c->data.ptr_value);
		switch (node->type) {

		case T_PGPropertyGraphTable: {
			auto graph_table = reinterpret_cast<PGPropertyGraphTable *>(c->data.ptr_value);
			auto qname = TransformQualifiedName(graph_table->name);
			auto pg_table = TranformPropertyGraphTable(graph_table);

			info->vertex_tables.push_back(move(pg_table));
			/*
			// info->schema = qname.schema;
			// info->name = qname.name;
			// auto pg_table = make_unique<PropertyGraphTable>();
			// pg_table->name = qname.name;
			// auto source_key_reference = TransformQualifiedName(graph_table->source_key_reference);

			// auto tableref = make_unique<BaseTableRef>();
			// tableref->table_name = graph_table->name->relname;
			// if (graph_table->name->schemaname) {
			// 	tableref->schema_name = graph_table->name->schemaname;
			// }
			// info->table_ref_list.push_back(move(tableref));
			// // ;
			// auto test = TransformRangeVar(graph_table->name);
			// // info->table_ref_list.push_back

			// // Will put these in a helper function
			// vector<string> keys, labels;
			// for (auto kc = graph_table->keys->head; kc; kc = kc->next) {
			// 	keys.push_back(string(reinterpret_cast<PGValue *>(kc->data.ptr_value)->val.str));
			// }

			// for (auto kc = graph_table->labels->head; kc; kc = kc->next) {
			// 	labels.push_back(string(reinterpret_cast<PGValue *>(kc->data.ptr_value)->val.str));
			// }

			// if(graph_table->is_vertex_table){
			// 	// info->vertex_tables.push_back()
			// 	info->vertex_tables.push_back(PropertyGraphTable(qname.name, keys, labels));
			// }
			// else{
			// 	for (auto kc = graph_table->source_key->head; kc; kc = kc->next) {
			// 		pg_table->source_key.push_back(string(reinterpret_cast<PGValue *>(kc->data.ptr_value)->val.str));
			// 	}

			// 	for (auto kc = graph_table->destination_key->head; kc; kc = kc->next) {
			// 		pg_table->destination_key.push_back(string(reinterpret_cast<PGValue
			*>(kc->data.ptr_value)->val.str));
			// 	}
			// 	//look into make_unique. Do we need to modfiy Copy
			// 	// info->edge_tables.push_back(pg_table);
			// }
			*/
			break;
		}
		default:
			throw NotImplementedException("ColumnDef type not handled yet");
		}
	}

	for (auto c = stmt->edge_tables->head; c != NULL; c = lnext(c)) {
		auto node = reinterpret_cast<PGNode *>(c->data.ptr_value);
		switch (node->type) {

		case T_PGPropertyGraphTable: {
			auto graph_table = reinterpret_cast<PGPropertyGraphTable *>(c->data.ptr_value);
			(void)graph_table;
			// auto qname = TransformQualifiedName(graph_table->name);
			// auto pg_table = TranformPropertyGraphTable(graph_table);
			// info->edge_tables.push_back(move(pg_table));
			break;
		}
		default:
			throw NotImplementedException("ColumnDef type not handled yet");
		}
	}

	result->info = move(info);
	return result;
}

} // namespace duckdb