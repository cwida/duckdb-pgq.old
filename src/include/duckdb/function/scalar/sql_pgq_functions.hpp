//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/function/scalar/sql_pgq_functions.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/function/scalar_function.hpp"
#include "duckdb/function/function_set.hpp"

namespace duckdb {

#define LANE_LIMIT 64
#define VISIT_SIZE_DIVISOR 2

struct CreateCsrFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct MsBfsFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

} // namespace duckdb
