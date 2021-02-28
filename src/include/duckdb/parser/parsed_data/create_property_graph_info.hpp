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

    //add second constructor with arguments ???
    
//add copy function
public:
    unique_ptr<CreateInfo> Copy() const override {
        auto result = make_unique<CreatePropertyGraphInfo>();
        CopyProperties(*result);
        return move(result);
    }
};

}  // namespace duckdb