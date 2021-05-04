#include "duckdb/common/exception.hpp"
#include "duckdb/parser/tableref.hpp"
#include "duckdb/parser/transformer.hpp"

namespace duckdb {

using namespace duckdb_libpgquery;

unique_ptr<TableRef> Transformer::TransformTableRefNode(PGNode *n) {
	switch (n->type) {
	case T_PGRangeVar:
		return TransformRangeVar(reinterpret_cast<PGRangeVar *>(n));
	case T_PGJoinExpr:
		return TransformJoin(reinterpret_cast<PGJoinExpr *>(n));
	case T_PGMatchPattern:
		// return TransformMatch(reinterpret_cast<>)
		return TransformMatch(reinterpret_cast<PGMatchPattern *>(n));
		// convert graph table match to subselect and then fall through
		// TransformMatch(reinterpret_cast<PGMatchPattern *>(n));
		// tables with src, dest combination and edge table --> kleene *
		// edge table
		// can have multiple kleene * ; path expressions where edge tables are views that are materilaised using
		// MATCH and not existing tables.
		// scalar udf for shortest path (vertex, edge pair, edge_table_name)
		// execute batched
		// edge table name represented using strings -> don't have multi table support udfs rn, so
		// injecting scalar function in binder --> embed query in binder to first execute create table in
		// is there a way to execute multiple queries for one query ?
		// table udfs -> 1 table in, 1 table out supported
		// represent
		// use with clause ; test even in SQL shortest path functionality (hacky albeit)
		// scalar_udf
		// edge table resolution -> under CTE or an existing edge tables
		// vertex table name, edge table name
		// on first call if it is not populated - we will put in commpact in memory representation,
		// expression _ state ? for array being used in the thien algo
		// al
		// weights specified in create_csr
		// run a sql query for creating csr to get proper edge table which is ordered an has dense mappings.
		// join edge table twice with thi modified vertex table, order by
		//
		// tabluar udf ? can allow it to return ptr -- to this array
		// execution problem --> cartesian product caused by select from q1, q2, q3 when only q3 results
		// reqd. could use count(q1), count(q2), q3. wrap this in
		// lateral join -- reqd ; maintains order and we can use alias instead of strings directly.
		// build_csr -> create in mem representation ; shortest path query refers.
		// size of vertex table, edge table : cleaned src, dest numbers
		// group

		// state important for cte/function
		// build_csr, shortest path in the extension module propose
		// 2 queries -> 1. build_csr wrap it in under count(*), 2. to do actual pgq query
	case T_PGRangeSubselect:
		return TransformRangeSubselect(reinterpret_cast<PGRangeSubselect *>(n));
	case T_PGRangeFunction:
		return TransformRangeFunction(reinterpret_cast<PGRangeFunction *>(n));

	default:
		throw NotImplementedException("From Type %d not supported yet...", n->type);
	}
}

} // namespace duckdb
