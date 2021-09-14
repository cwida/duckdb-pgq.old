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

struct CreateCsrFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

} // namespace duckdb
