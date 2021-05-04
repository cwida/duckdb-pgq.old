//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/planner/tableref/bound_joinref.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/planner/bound_tableref.hpp"
// #include "duckdb/planner/expression.hpp"

namespace duckdb {

//! Represents a join
class BoundMatchRef : public BoundTableRef {
public:
	BoundMatchRef() : BoundTableRef(TableReferenceType::MATCH) {
	}

	// //! The binder used to bind the LHS of the join
	// unique_ptr<Binder> left_binder;
	// //! The binder used to bind the RHS of the join
	// unique_ptr<Binder> right_binder;
	// //! The left hand side of the join
	// unique_ptr<BoundTableRef> left;
	// //! The right hand side of the join
	// unique_ptr<BoundTableRef> right;
	// //! The join condition
	// unique_ptr<Expression> condition;
	//! The join type
	// JoinType type;
};
} // namespace duckdb
