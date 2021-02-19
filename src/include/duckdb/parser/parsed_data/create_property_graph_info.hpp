//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/parsed_data/create_property_graph_info.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/parser/parsed_data/create_property_graph_info.hpp"

namespace duckdb {

struct CreatePropertyGraphInfo : public CreateInfo {
    CreatePropertyGraphInfo() : CreateInfo(CatalogType::PROPERTY_GRAPH_ENTRY) {
	}
}
