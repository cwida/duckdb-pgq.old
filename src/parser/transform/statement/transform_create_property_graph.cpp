#include "duckdb/parser/statement/create_statement.hpp"
#include "duckdb/parser/transformer.hpp"
#include "duckdb/parser/parsed_data/create_property_graph_info.hpp"

namespace duckdb {

using namespace duckdb_libpgquery;

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
		switch (node->type){

		case T_PGPropertyGraphTable:{
			auto graph_table = reinterpret_cast<PGPropertyGraphTable *>(c->data.ptr_value);
			auto qname = TransformQualifiedName(graph_table->name);
			// info->schema = qname.schema;
			// info->name = qname.name;
			auto pg_table = make_unique<PropertyGraphTable>();
			pg_table->name = qname.name;
			auto source_key_reference = TransformQualifiedName(graph_table->source_key_reference);
			// Will put these in a helper function 
			vector<string> keys, labels;
			for (auto kc = graph_table->keys->head; kc; kc = kc->next) {
				keys.push_back(string(reinterpret_cast<PGValue *>(kc->data.ptr_value)->val.str));
			}

			for (auto kc = graph_table->labels->head; kc; kc = kc->next) {
				labels.push_back(string(reinterpret_cast<PGValue *>(kc->data.ptr_value)->val.str));
			}
			
			if(graph_table->is_vertex_table){
				// info->vertex_tables.push_back()
				info->vertex_tables.push_back(PropertyGraphTable(qname.name, keys, labels));	
			}
			else{
				for (auto kc = graph_table->source_key->head; kc; kc = kc->next) {
					pg_table->source_key.push_back(string(reinterpret_cast<PGValue *>(kc->data.ptr_value)->val.str));
				}

				for (auto kc = graph_table->destination_key->head; kc; kc = kc->next) {
					pg_table->destination_key.push_back(string(reinterpret_cast<PGValue *>(kc->data.ptr_value)->val.str));
				}
				//look into make_unique. Do we need to modfiy Copy
				// info->edge_tables.push_back(pg_table);
			}
		}
		default:
			throw NotImplementedException("ColumnDef type not handled yet");
		}
	}

	result->info = move(info);
	return result;
}

} // namespace duckdb