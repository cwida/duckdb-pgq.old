#include "duckdb/execution/operator/schema/physical_create_property_graph.hpp"
#include "duckdb/catalog/catalog.hpp"

namespace duckdb {

void PhysicalCreatePropertyGraph::GetChunkInternal(ExecutionContext &context, DataChunk &chunk,
                                                   PhysicalOperatorState *state) {
	// Catalog::GetCatalog(context.client).CreatePropertyGraph(context.client, info.get());
	state->finished = true;
}

} // namespace duckdb
