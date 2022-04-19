#define DUCKDB_EXTENSION_MAIN
#include "sqlpgq-extension.hpp"

#include "duckdb/catalog/catalog_entry/macro_catalog_entry.hpp"
#include "duckdb/catalog/default/default_functions.hpp"
#include "sqlpgq_functions.hpp"
#include "duckdb.hpp"
#include <iostream>


namespace duckdb {


void SQLPGQExtension::Load(DuckDB &db) {
	Connection con(db);
	con.BeginTransaction();

	auto &catalog = Catalog::GetCatalog(*con.context);
	for (auto &fun : SQLPGQFunctions::GetFunctions()) {
		catalog.CreateFunction(*con.context, &fun);
	}

//	for (idx_t index = 0; json_macros[index].name != nullptr; index++) {
//		auto info = DefaultFunctionGenerator::CreateInternalMacroInfo(json_macros[index]);
//		catalog.CreateFunction(*con.context, info.get());
//	}

	con.Commit();
}

std::string SQLPGQExtension::Name() {
	return "sqlpgq";
}

//std::string SQLPGQExtension::Name() {
//	return "sqlpgq";
//}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void sqlpgq_init(duckdb::DatabaseInstance &db) {
	duckdb::DuckDB db_wrapper(db);
	db_wrapper.LoadExtension<duckdb::SQLPGQExtension>();
}

DUCKDB_EXTENSION_API const char *sqlpgq_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
