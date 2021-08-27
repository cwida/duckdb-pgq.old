#include "duckdb/execution/operator/persistent/physical_insert.hpp"
#include "duckdb/parallel/thread_context.hpp"
#include "duckdb/catalog/catalog_entry/table_catalog_entry.hpp"
#include "duckdb/common/types/chunk_collection.hpp"
#include "duckdb/common/vector_operations/vector_operations.hpp"
#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/storage/data_table.hpp"
#include "duckdb/main/client_context.hpp"

namespace duckdb {

//===--------------------------------------------------------------------===//
// Sink
//===--------------------------------------------------------------------===//
class InsertGlobalState : public GlobalSinkState {
public:
	InsertGlobalState() : insert_count(0) {
	}

	mutex lock;
	idx_t insert_count;
};

class InsertLocalState : public LocalSinkState {
public:
	InsertLocalState(const vector<LogicalType> &types, const vector<unique_ptr<Expression>> &bound_defaults)
	    : default_executor(bound_defaults) {
		insert_chunk.Initialize(types);
	}

	DataChunk insert_chunk;
	ExpressionExecutor default_executor;
};

void PhysicalInsert::Sink(ExecutionContext &context, GlobalSinkState &state, LocalSinkState &lstate,
                          DataChunk &chunk) const {
	auto &gstate = (InsertGlobalState &)state;
	auto &istate = (InsertLocalState &)lstate;

	chunk.Normalify();
	istate.default_executor.SetChunk(chunk);

	istate.insert_chunk.Reset();
	istate.insert_chunk.SetCardinality(chunk);
	if (!column_index_map.empty()) {
		// columns specified by the user, use column_index_map
		for (idx_t i = 0; i < table->columns.size(); i++) {
			if (column_index_map[i] == INVALID_INDEX) {
				// insert default value
				istate.default_executor.ExecuteExpression(i, istate.insert_chunk.data[i]);
			} else {
				// get value from child chunk
				D_ASSERT((idx_t)column_index_map[i] < chunk.ColumnCount());
				D_ASSERT(istate.insert_chunk.data[i].GetType() == chunk.data[column_index_map[i]].GetType());
				istate.insert_chunk.data[i].Reference(chunk.data[column_index_map[i]]);
			}
		}
	} else {
		// no columns specified, just append directly
		for (idx_t i = 0; i < istate.insert_chunk.ColumnCount(); i++) {
			D_ASSERT(istate.insert_chunk.data[i].GetType() == chunk.data[i].GetType());
			istate.insert_chunk.data[i].Reference(chunk.data[i]);
		}
	}

	lock_guard<mutex> glock(gstate.lock);
	table->storage->Append(*table, context.client, istate.insert_chunk);
	gstate.insert_count += chunk.size();
}

unique_ptr<GlobalSinkState> PhysicalInsert::GetGlobalSinkState(ClientContext &context) const {
	return make_unique<InsertGlobalState>();
}

unique_ptr<LocalSinkState> PhysicalInsert::GetLocalSinkState(ExecutionContext &context) const {
	return make_unique<InsertLocalState>(table->GetTypes(), bound_defaults);
}

void PhysicalInsert::Combine(ExecutionContext &context, GlobalSinkState &gstate, LocalSinkState &lstate) const {
	auto &state = (InsertLocalState &)lstate;
	context.thread.profiler.Flush(this, &state.default_executor, "default_executor", 1);
	context.client.profiler->Flush(context.thread.profiler);
}

//===--------------------------------------------------------------------===//
// Source
//===--------------------------------------------------------------------===//
class InsertSourceState : public GlobalSourceState {
public:
	InsertSourceState() : finished(false) {}

	bool finished;
};

unique_ptr<GlobalSourceState> PhysicalInsert::GetGlobalSourceState(ClientContext &context) const {
	return make_unique<InsertSourceState>();
}

void PhysicalInsert::GetData(ExecutionContext &context, DataChunk &chunk, GlobalSourceState &gstate, LocalSourceState &lstate) const {
	auto &state = (InsertSourceState &) gstate;
	auto &g = (InsertGlobalState &)*sink_state;
	if (state.finished) {
		return;
	}

	chunk.SetCardinality(1);
	chunk.SetValue(0, 0, Value::BIGINT(g.insert_count));
	state.finished = true;
}

} // namespace duckdb
