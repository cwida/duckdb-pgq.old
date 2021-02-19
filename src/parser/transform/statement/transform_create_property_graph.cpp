#include "duckdb/parser/statement/create_statement.hpp"
#include "duckdb/parser/transformer.hpp"
#include "duckdb/parser/parsed_data/create_view_info.hpp"

namespace duckdb {

using namespace duckdb_libpgquery;

unique_ptr<CreateStatement> Transformer::TransformCreateView(PGNode *node) {
