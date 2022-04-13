#include "duckdb/function/scalar/sql_pgq_functions.hpp"

namespace duckdb {

void BuiltinFunctions::RegisterSqlPgqFunctions() {
	Register<CreateCsrFun>();
	Register<MsBfsFun>();
	Register<ShortestPathFun>();
}

} // namespace duckdb
