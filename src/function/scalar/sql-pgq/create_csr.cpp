#include "duckdb/function/scalar/sql_pgq_functions.hpp"
#include "duckdb/main/client_context.hpp"

namespace duckdb {

//might need a struct for bound data
/*struct StatsBindData : public FunctionData {
	StatsBindData(string stats = string()) : stats(move(stats)) {
	}

	string stats;

public:
	unique_ptr<FunctionData> Copy() override {
		return make_unique<StatsBindData>(stats);
	}
};*/

struct CsrBindData : public FunctionData {
	ClientContext &context;

	CurrentBindData(ClientContext &context) : context(context) {
	}

	unique_ptr<FunctionData> Copy() override {
		return make_unique<CsrBindData>(context);
	}
};

//will passing context work directly, or do I need to ppass state again?
static void csr_initialize_vertex_or_edge(DataChunk &args, ExpressionState &state, bool is_vertex) {
	// auto &func_expr = (BoundFunctionExpression &)state.expr;
	// auto &info = (CsrBindData &)*func_expr.bind_info;
	
	auto csr = info.context.csr_list[args.data[0]];
	if(is_vertex) {
		lock_guard<mutex> csr_init_lock(seq->lock);
		if(info.context.initialized_v){
			return;
		}
		try{
			//extra 2 spaces required for CSR padding
			csr.v.resize(args.data[1] + 2, 0);
		}
		catch (std::bad_alloc const&) {
			throw Exception("Unable to initialise vector of size %d for csr vertex table representation", args.data[1] + 2);
		}
		info.context.initialized_v = true;
		return;
	}
	else {
		lock_guard<mutex> csr_init_lock(seq->lock);
		if(info.context.initialized_e){
			return;
		}
		try{
			//extra 2 spaces required for CSR padding
			csr.e.resize(args.data[1] + 2, 0);
		}
		catch (std::bad_alloc const&) {
			throw Exception("Unable to initialise vector of size %d for csr edge table representation", args.data[1] + 2);
		}
		info.context.initialized_e = true;
		//create running sum
		for (auto i = 1; i < args.data[1]; i++) {
			csr.v[i] += csr.v[i-1];
		}
		return; 
	}

}



// static void csr_initilaize_v(DataChunk args, )
static void create_csr_vertex_function(DataChunk &args, ExpressionState &state, Vector &result) {
	D_ASSERT(args.ColumnCount() == 0);
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

	auto csr = info.context.csr[args.data[0]];

	if(!info.context.initialized_v){
		csr_initialize_vertex_or_edge(args, info.context, true);
	}
	

	// Value v(args.data[0].type.ToString());
	edge_count = 0;
	for(int i = 0; i < args.data[2].size(); i++) {
		std::atomic(csr.v[args.data[2][i+2]], 1);
		edge_count++;
	}

	// result.Reference(v);
	// return 
	result + = edge_count;
	return;
}

static int create_csr_edge_function(DataChunk &args, ExpressionState &state, Vector &result) {
	D_ASSERT(args.ColumnCount() == 0);
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;
	
	auto csr = info.context.csr[args.data[0]];

	if(!info.context.initialized_e){
		csr_initialize_vertex_or_edge(args, info.context, false);
	}

	for(int i = 0; i < args.data[3].size(); i++) {
		auto pos = std::atomic(csr.v[args.data[3][i] + 1], 1);
		csr.e[pos - 1] = args.data[4][i];
	}

	// result.Reference(v);
	return edge_count;
}


//decide on function return type
AggregateFunction CreateCsrFun::GetFunction() {
	return AggregateFunction::NullaryAggregate<int64_t, int64_t, CreateCsrFunction>(LogicalType::BIGINT);
}

void TypeOfFun::RegisterFunction(BuiltinFunctions &set) {
	//no need for bind function as data not modified
	//args -> id, |V|, src
	set.AddFunction(ScalarFunction("create_csr_vertex", {LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::List}, LogicalType::INTEGER, create_csr_vertex_function));

	//figure out how to return type void
	// args -> id, |V|, |E|, src, dst
	set.AddFunction(ScalarFunction("create_csr_edge", {LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::INTEGER, LogicalType::List, LogicalType::List}, LogicalType::ANY, create_csr_edge_function));


	// set.AddFunction(ScalarFunction("strlen", {LogicalType::VARCHAR}, LogicalType::BIGINT,
	//                                ScalarFunction::UnaryFunction<string_t, int64_t, StrLenOperator, true>));
}

} // namespace duckdb
