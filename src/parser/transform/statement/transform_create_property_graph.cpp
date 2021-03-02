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

	result->info = move(info);
	return result;
}

} // namespace duckdb