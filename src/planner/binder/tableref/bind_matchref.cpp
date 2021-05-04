#include "duckdb/parser/tableref/matchref.hpp"
#include "duckdb/planner/binder.hpp"
#include "duckdb/planner/tableref/bound_matchref.hpp"

namespace duckdb {

unique_ptr<BoundTableRef> Binder::Bind(MatchRef &ref) {
	return make_unique<BoundMatchRef>();
}

} // namespace duckdb
