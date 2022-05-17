//
// Created by daniel on 19-4-22.
//

#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/planner/expression/bound_function_expression.hpp"
#include "sqlpgq_functions.hpp"

#include <iostream>

namespace duckdb {

struct CsrBindData : public FunctionData {
	ClientContext &context;
	int32_t id;
	bool weighted = false;
	//	int64_t num_of_vertices = 0;
	//	int64_t num_of_edges = 0;

	CsrBindData(ClientContext &context, int32_t id, bool weighted) : context(context), id(id), weighted(weighted) {
	}

	unique_ptr<FunctionData> Copy() override {
		return make_unique<CsrBindData>(context, id, weighted);
	}
};

static void CsrInitializeVertex(ClientContext &context, int32_t id, int64_t v_size) {
	lock_guard<mutex> csr_init_lock(context.csr_lock);
	try {
		auto csr = make_unique<Csr>();
		// extra 2 spaces required for CSR padding
		// data contains a vector of elements so will need an anonymous function to apply the
		// the first element id is repeated across, can I access the value directly?
		csr->v = new std::atomic<int64_t>[v_size + 2];
		csr->v_weight = new std::atomic<int64_t>[v_size + 2];

		for (idx_t i = 0; i < (idx_t)v_size + 2; i++) {
			csr->v[i] = 0;
			csr->v_weight[i] = 0;
		}
		if (((u_int64_t)id) < context.csr_list.size()) {
			context.csr_list[id] = move(csr);
		} else {
			context.csr_list.push_back(move(csr));
		}
	} catch (std::bad_alloc const &) {
		throw Exception("Unable to initialise vector of size for csr vertex table representation");
	}
	context.initialized_v = true;
	return;
}

static void CsrInitializeEdge(ClientContext &context, int32_t id, int64_t v_size, int64_t e_size) {
	lock_guard<mutex> csr_init_lock(context.csr_lock);
	if (context.initialized_e) {
		return;
	}
	try {
		context.csr_list[id]->e.resize(e_size, 0);
	} catch (std::bad_alloc const &) {
		throw Exception("Unable to initialise vector of size for csr edge table representation");
	}

//	for (auto i = 1; i < v_size + 2; i++) {
	//		context.csr_list[id]->v[i] += context.csr_list[id]->v[i - 1];
	//	}
	context.initialized_e = true;
	return;
}

static void CsrInitializeWeight(ClientContext &context, int32_t id, int64_t v_size, int64_t e_size,
                                PhysicalType weight_type) {
	lock_guard<mutex> csr_init_lock(context.csr_lock);
	if (context.initialized_w) {
		return;
	}
	try {
		if (weight_type == PhysicalType::INT64) {
			context.csr_list[id]->w.resize(e_size, 0);
		} else if (weight_type == PhysicalType::DOUBLE) {
			context.csr_list[id]->w_double.resize(e_size, 0);
		} else {
			throw NotImplementedException("Unrecognized weight type detected.");
		}
	} catch (std::bad_alloc const &) {
		throw Exception("Unable to initialise vector of size for csr weight table representation");
	}

	for (auto i = 1; i < v_size + 2; i++) {
		context.csr_list[id]->v_weight[i] += context.csr_list[id]->v_weight[i - 1];
	}
	context.initialized_w = true;
	return;
}

static unique_ptr<FunctionData> CreateCsrVertexBind(ClientContext &context, ScalarFunction &bound_function,
                                                    vector<unique_ptr<Expression>> &arguments) {
	if (!arguments[0]->IsFoldable()) {
		throw InvalidInputException("Id must be constant.");
	}

	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);

	return make_unique<CsrBindData>(context, id.GetValue<int32_t>(), false);
}

static unique_ptr<FunctionData> CreateCsrBind(ClientContext &context, ScalarFunction &bound_function,
                                              vector<unique_ptr<Expression>> &arguments) {
	if (!arguments[0]->IsFoldable()) {
		throw InvalidInputException("Id must be constant.");
	}

	child_list_t<LogicalType> struct_children;

	struct_children.push_back(make_pair("id", LogicalType::INTEGER));
	struct_children.push_back(make_pair("vertices", LogicalType::BIGINT));
	struct_children.push_back(make_pair("edges", LogicalType::BIGINT));
	struct_children.push_back(make_pair("weight", LogicalType::VARCHAR));

	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);
	bound_function.return_type = LogicalType::STRUCT(move(struct_children));
	return make_unique<CsrBindData>(context, id.GetValue<int32_t>(), false); // TODO Add unweighted version as well
}

static void CreateCsrFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

//	SelectionVector

	int64_t input_size = args.data[1].GetValue(0).GetValue<int64_t>();
	int64_t edge_size = args.data[2].GetValue(0).GetValue<int64_t>();

	CsrInitializeVertex(info.context, info.id, input_size);
	CsrInitializeEdge(info.context, info.id, input_size, edge_size);
	args.data[3].SetVectorType(VectorType::FLAT_VECTOR);
	args.data[4].SetVectorType(VectorType::FLAT_VECTOR);
	args.data[6].SetVectorType(VectorType::FLAT_VECTOR);


	for (idx_t i = 0; i < args.size(); i += args.size()) {
		Vector row_vector(args.data[3], i);
		Vector cnt_vector(args.data[4], i);
		Vector dst_vector(args.data[6], i);



		auto &child_entries = StructVector::GetEntries(result);
//		int64_t last_src_set = -1;

		TernaryExecutor::Execute<int64_t, int64_t, int64_t, int64_t>(
			row_vector, cnt_vector, dst_vector, *child_entries[1], args.size(),
			[&](int64_t src, int64_t cnt, int64_t dst) {
//				if (src != last_src_set) {
//					if (last_src_set != -1) {
//						info.context.csr_list[info.id]->v[src+1] += info.context.csr_list[info.id]->v[src-1];
//					}
//					last_src_set = src;
//					info.context.csr_list[info.id]->v[src + 2] = cnt;
//					info.context.csr_list[info.id]->v_weight[src + 2] = cnt;
//				}
//				int64_t pos = ++info.context.csr_list[info.id]->v[src + 1];
				std::cout << "src: " << src << "\tcnt: " << cnt << "\tdst: " << dst  << std::endl; // << "\tpos:" << pos
//				info.context.csr_list[info.id]->e[pos - 1] = dst;
				return 1;
			});

	}

//	bool weighted = false;
//	if (args.data.size() == 8) { //! 7th argument defines the weights of the edges. If there are 6 arguments, the edges are unweighted.
//		weighted = true;
//	}
//

//
//
//
//	auto &child_entries = StructVector::GetEntries(result);
//	int64_t last_src_set = -1;
//
//
//	TernaryExecutor::Execute<int64_t, int64_t, int64_t, int64_t>(
//	    args.data[3], args.data[4], args.data[6], *child_entries[1], args.size(),
//	    [&](int64_t src, int64_t cnt, int64_t dst) {
//		    if (src != last_src_set) {
//			    if (last_src_set != -1) {
//				    info.context.csr_list[info.id]->v[src+1] += info.context.csr_list[info.id]->v[src-1];
//			    }
//			    last_src_set = src;
//				info.context.csr_list[info.id]->v[src + 2] = cnt;
//			    info.context.csr_list[info.id]->v_weight[src + 2] = cnt;
//		    }
//		    int64_t pos = ++info.context.csr_list[info.id]->v[src + 1];
//		    std::cout << "src: " << src << "\tcnt: " << cnt << "\tdst: " << dst << "\tpos:" << pos << std::endl;
//		    info.context.csr_list[info.id]->e[pos - 1] = dst;
//		    return pos;
//	    });

//	BinaryExecutor::Execute<int64_t, int64_t, int64_t>(args.data[3], args.data[4], *child_entries[1], args.size(),
//	                                                   [&](int64_t src, int64_t cnt) {
//		                                                   std::cout << "src: " << src << " cnt: " << cnt << std::endl;
//		                                                   info.context.csr_list[info.id]->v[src + 2] = cnt;
//		                                                   info.context.csr_list[info.id]->v_weight[src + 2] = cnt;
//		                                                   return cnt;
//	                                                   });
//
//
//
//
//
//	std::cout << std::endl;
//
//	BinaryExecutor::Execute<int64_t, int64_t, int32_t>(args.data[5], args.data[6], *child_entries[2], args.size(),
//	                                                   [&](int64_t src, int64_t dst) {
//		                                                   int64_t pos = ++info.context.csr_list[info.id]->v[src + 1];
//		                                                   info.context.csr_list[info.id]->e[pos - 1] = dst;
//		                                                   return pos;
//	                                                   });
//	if (weighted) {
//		auto weight_type = args.data[7].GetType().InternalType();
//		CsrInitializeWeight(info.context, info.id, input_size, edge_size, weight_type);
//		if (weight_type == PhysicalType::INT64) {
//			BinaryExecutor::Execute<int64_t, int64_t, int32_t>(
//			    args.data[5], args.data[7], *child_entries[2], args.size(), [&](int64_t src, int64_t weight) {
//				    auto pos = ++info.context.csr_list[info.id]->v_weight[src + 1];
//				    info.context.csr_list[info.id]->w[(int64_t)pos - 1] = weight;
//				    return 1;
//			    });
//			child_entries[3]->Reference(Value("integer"));
//
//		} else if (weight_type == PhysicalType::DOUBLE) {
//			BinaryExecutor::Execute<int64_t, double_t, int32_t>(
//			    args.data[5], args.data[7], *child_entries[2], args.size(), [&](int64_t src, double_t weight) {
//				    auto pos = ++info.context.csr_list[info.id]->v_weight[src + 1];
//				    info.context.csr_list[info.id]->w_double[(int64_t)pos - 1] = weight;
//				    return 1;
//			    });
//			child_entries[3]->Reference(Value("double"));
//		}
//	}
//
//	result.SetVectorType(VectorType::CONSTANT_VECTOR);
//	child_entries[0]->Reference(args.data[0]);
//	child_entries[1]->Reference(Value(input_size));
//	child_entries[2]->Reference(Value((int64_t)args.size()));
//	if (!weighted) { // TODO Test if works.
//		child_entries[3]->Reference(Value("none"));
//	}
	return;
}

static void CreateCsrVertexFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

	int64_t input_size = args.data[1].GetValue(0).GetValue<int64_t>();
	if (!info.context.initialized_v) {
		CsrInitializeVertex(info.context, info.id, input_size);
	}
	BinaryExecutor::Execute<int64_t, int64_t, int64_t>(args.data[2], args.data[3], result, args.size(),
	                                                   [&](int64_t src, int64_t cnt) {
		                                                   int64_t edge_count = 0;
		                                                   info.context.csr_list[info.id]->v[src + 2] = cnt;
		                                                   info.context.csr_list[info.id]->v_weight[src + 2] = cnt;
		                                                   edge_count = edge_count + cnt;
		                                                   return edge_count;
	                                                   });

	return;
}

static void CreateCsrEdgeFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (CsrBindData &)*func_expr.bind_info;

	int64_t vertex_size = args.data[1].GetValue(0).GetValue<int64_t>();
	int64_t edge_size = args.data[2].GetValue(0).GetValue<int64_t>();
	CsrInitializeEdge(info.context, info.id, vertex_size, edge_size);

	BinaryExecutor::Execute<int64_t, int64_t, int32_t>(
	    args.data[3], args.data[4], result, args.size(), [&](int64_t src, int64_t dst) {
		    auto pos = ++info.context.csr_list[info.id]->v[src + 1];
//		    std::cout << "Edge: pos: " << pos << "\tdst: " << dst << "\tsrc: " << src << std::endl;
		    info.context.csr_list[info.id]->e[(int64_t)pos - 1] = dst;
		    return 1;
	    });
	if (info.weighted) {
		CsrInitializeWeight(info.context, info.id, vertex_size, edge_size, args.data[5].GetType().InternalType());
		BinaryExecutor::Execute<int64_t, int64_t, int32_t>(
		    args.data[3], args.data[5], result, args.size(), [&](int64_t src, int64_t weight) {
			    auto pos = ++info.context.csr_list[info.id]->v_weight[src + 1];
			    info.context.csr_list[info.id]->w[(int64_t)pos - 1] = weight;

			    return 1;
		    });
	}

	return;
}

static unique_ptr<FunctionData> CreateCsrEdgeBind(ClientContext &context, ScalarFunction &bound_function,
                                                  vector<unique_ptr<Expression>> &arguments) {
	if (!arguments[0]->IsFoldable()) {
		throw InvalidInputException("Id must be constant.");
	}
	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);
	if (arguments.size() == 5) {
		return make_unique<CsrBindData>(context, id.GetValue<int32_t>(), false);
	} else {
		return make_unique<CsrBindData>(context, id.GetValue<int32_t>(), true);
	}
}

static unique_ptr<FunctionData> CreateCsrWeightBind(ClientContext &context, ScalarFunction &bound_function,
                                                    vector<unique_ptr<Expression>> &arguments) {
	if (!arguments[0]->IsFoldable()) {
		throw InvalidInputException("Id must be constant.");
	}
	Value id = ExpressionExecutor::EvaluateScalar(*arguments[0]);
	return make_unique<CsrBindData>(context, id.GetValue<int32_t>(), true);
}

CreateScalarFunctionInfo SQLPGQFunctions::GetCsrVertexFunction() {
	auto fun = ScalarFunction("create_csr_vertex",
	                          {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	                          LogicalType::BIGINT, CreateCsrVertexFunction, false, CreateCsrVertexBind);
	return CreateScalarFunctionInfo(fun);
}

CreateScalarFunctionInfo SQLPGQFunctions::GetCsrFunction() {
	ScalarFunctionSet set("create_csr");

	set.AddFunction(ScalarFunction( //! Integers as edge weights
	    {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT,
	     LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalTypeId::STRUCT, CreateCsrFunction, false, CreateCsrBind));
	set.AddFunction(ScalarFunction( //! Double as edge weights
	    {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT,
	     LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::DOUBLE},
	    LogicalTypeId::STRUCT, CreateCsrFunction, false, CreateCsrBind));
	set.AddFunction(ScalarFunction( //! Unweighted variation
	    {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT,
	     LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalTypeId::STRUCT, CreateCsrFunction, false, CreateCsrBind));
	return CreateScalarFunctionInfo(set);
}

CreateScalarFunctionInfo SQLPGQFunctions::GetCsrEdgeFunction() {
	ScalarFunctionSet set("create_csr_edge");
	set.AddFunction(ScalarFunction(
	    {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::INTEGER, CreateCsrEdgeFunction, false, CreateCsrEdgeBind));
	set.AddFunction(ScalarFunction({LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT,
	                                LogicalType::BIGINT, LogicalType::BIGINT},
	                               LogicalType::INTEGER, CreateCsrEdgeFunction, false, CreateCsrWeightBind));
	set.AddFunction(ScalarFunction({LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT,
	                                LogicalType::BIGINT, LogicalType::DOUBLE},
	                               LogicalType::INTEGER, CreateCsrEdgeFunction, false, CreateCsrWeightBind));

	return CreateScalarFunctionInfo(set);
}
} // namespace duckdb
