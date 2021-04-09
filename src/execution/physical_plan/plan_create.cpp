#include "duckdb/execution/physical_plan_generator.hpp"
#include "duckdb/planner/logical_operator.hpp"
#include "duckdb/planner/operator/logical_create.hpp"

#include "duckdb/execution/operator/schema/physical_create_schema.hpp"
#include "duckdb/execution/operator/schema/physical_create_sequence.hpp"
#include "duckdb/execution/operator/schema/physical_create_view.hpp"
#include "duckdb/execution/operator/schema/physical_create_function.hpp"
#include "duckdb/execution/operator/schema/physical_create_property_graph.hpp"

namespace duckdb {

unique_ptr<PhysicalOperator> PhysicalPlanGenerator::CreatePlan(LogicalCreate &op) {
	switch (op.type) {
	case LogicalOperatorType::LOGICAL_CREATE_SEQUENCE:
		return make_unique<PhysicalCreateSequence>(unique_ptr_cast<CreateInfo, CreateSequenceInfo>(move(op.info)));
	case LogicalOperatorType::LOGICAL_CREATE_VIEW:
		return make_unique<PhysicalCreateView>(unique_ptr_cast<CreateInfo, CreateViewInfo>(move(op.info)));
	case LogicalOperatorType::LOGICAL_CREATE_SCHEMA:
		return make_unique<PhysicalCreateSchema>(unique_ptr_cast<CreateInfo, CreateSchemaInfo>(move(op.info)));
	case LogicalOperatorType::LOGICAL_CREATE_MACRO:
		return make_unique<PhysicalCreateFunction>(unique_ptr_cast<CreateInfo, CreateMacroInfo>(move(op.info)));
	case LogicalOperatorType::LOGICAL_CREATE_PROPERTY_GRAPH:
		return make_unique<PhysicalCreatePropertyGraph>(
		    unique_ptr_cast<CreateInfo, CreatePropertyGraphInfo>(move(op.info)));
	default:
		throw NotImplementedException("Unimplemented type for logical simple create");
	}
}

} // namespace duckdb
