//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/execution/operator/schema/physical_create_view.hpp
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
	PhysicalCreatePropertyGraph(unique_ptr<CreatePropertyGraphInfo> info)
	    : PhysicalOperator(PhysicalOperatorType::CREATE_PROPERTY_GRAPH, {LogicalType::BIGINT}), info(move(info)) {
	}

	unique_ptr<CreatePropertyGraphInfo> info;

public:
	void GetChunkInternal(ExecutionContext &context, DataChunk &chunk, PhysicalOperatorState *state) override;
};

} // namespace duckdb