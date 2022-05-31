//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/execution/operator/schema/physical_create_property_Graph_info.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/execution/physical_operator.hpp"
#include "duckdb/parser/parsed_data/create_property_graph_info.hpp"

namespace duckdb {

//! PhysicalCreatePropertyGraph represents a CREATE PROPERTY_GRAPH command
class PhysicalCreatePropertyGraph : public PhysicalOperator {
public:
	explicit PhysicalCreatePropertyGraph(unique_ptr<CreatePropertyGraphInfo> info, idx_t estimated_cardinality)
	    : PhysicalOperator(PhysicalOperatorType::CREATE_PROPERTY_GRAPH, {LogicalType::BIGINT}, estimated_cardinality),
	      info(move(info)) {
	}

	unique_ptr<CreatePropertyGraphInfo> info;

public:
	// Source interface
	unique_ptr<GlobalSourceState> GetGlobalSourceState(ClientContext &context) const override;
	void GetData(ExecutionContext &context, DataChunk &chunk, GlobalSourceState &gstate,
	             LocalSourceState &lstate) const override;
};

} // namespace duckdb
