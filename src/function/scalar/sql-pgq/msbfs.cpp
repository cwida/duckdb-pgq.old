#include "duckdb/function/scalar/sql_pgq_functions.hpp"
#include "duckdb/planner/expression/bound_function_expression.hpp"
#include "duckdb/common/vector_operations/vector_operations.hpp"
#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/common/vector_operations/vector_operations.hpp"
#include "duckdb/common/pair.hpp"
#include "duckdb/common/fstream.hpp"
#include "duckdb/common/profiler.hpp"

namespace duckdb {

typedef enum MsbfsModes {
	NO_ARRAY,
	ARRAY,
	INTERMEDIATE
} MsbfsModes;

struct MsbfsBindData : public FunctionData {
	ClientContext &context;
	string file_name;
	// int32_t num_bfs;

	MsbfsBindData(ClientContext &context, string &file_name) : context(context), file_name(file_name) {
	}


	~MsbfsBindData() {
		// delete [] context.csr_list[0]->v;
	}

	unique_ptr<FunctionData> Copy() override {
		return make_unique<MsbfsBindData>(context, file_name);
	}
};

// static bool check_empty_bitset(vector<int64_t> set_list) {
// 	bool ans = true;
// 	// int64_t sum = 0;
// 	for(auto set: set_list) {
// 		// sum = sum + set;
// 		if(set > 0) {
// 			ans = false;
// 			break;
// 		}
// 	}
// 	return ans;
// }

// static bool is_bit_set(uint32_t num, uint8_t bit)
// {
//     return 1 == ( (num >> bit) & 1);
// }

static int16_t initialise_bfs(idx_t curr_batch, idx_t size, int64_t *src_data, const SelectionVector *src_sel,  
ValidityMask src_validity, vector<std::bitset<LANE_LIMIT>> &seen,
		vector<std::bitset<LANE_LIMIT>> &visit,
		vector<std::bitset<LANE_LIMIT>> &visit_next,
		unordered_map<int64_t, pair<int16_t, vector<int64_t>>> &lane_map) {
			// 
	int16_t lanes = 0;
	int16_t curr_batch_size = 0;
	
	for(idx_t i =  curr_batch; i < size && lanes < LANE_LIMIT  ; i++) {
		auto src_index = src_sel->get_index(i);
		
		if (src_validity.RowIsValid(src_index)) { 
			auto entry = lane_map.find(src_data[src_index]);
			if(entry == lane_map.end()) {
				lane_map[src_data[src_index]].first = lanes;
				// seen[src_data[i]] = std::bitset<LANE_LIMIT>();
				seen[src_data[src_index]][lanes] = 1;
				// visit[src_data[i]] = std::bitset<LANE_LIMIT>();
				visit[src_data[src_index]][lanes] = 1;
				lanes++;
			}
				lane_map[src_data[src_index]].second.push_back(i);
				curr_batch_size++;
		}
	}
	return curr_batch_size;
}

static bool bfs_without_array_variant(bool exit_early, int32_t id, int64_t input_size, MsbfsBindData &info, vector<std::bitset<LANE_LIMIT>> &seen, 
		vector<std::bitset<LANE_LIMIT>> &visit,
		vector<std::bitset<LANE_LIMIT>> &visit_next, vector<int64_t> &visit_list) { 
	for (int64_t i = 0; i < input_size; i++) {
		if (!visit[i].any())
			continue;
		
		D_ASSERT(info.context.csr_list[id]);
		for (auto index = (long)info.context.csr_list[id]->v[i]; index < (long)info.context.csr_list[id]->v[i + 1]; index++) {
			auto n = info.context.csr_list[id]->e[index];
			visit_next[n] = visit_next[n] | visit[i];
			
		}
		
	}
	
	for (int64_t i = 0; i < input_size ; i++) {
		if (visit_next[i].none())
			continue;
		visit_next[i] = visit_next[i] & ~seen[i];
		seen[i] = seen[i] | visit_next[i];
		if(exit_early == true && visit_next[i].any() )
			exit_early = false;
		if(visit_next[i].any()){
			visit_list.push_back(i);
		}
			
	}
	return exit_early;
}

static bool bfs_without_array(bool exit_early, int32_t id, int64_t input_size, MsbfsBindData &info, vector<std::bitset<LANE_LIMIT>> &seen, 
		vector<std::bitset<LANE_LIMIT>> &visit,
		vector<std::bitset<LANE_LIMIT>> &visit_next) { 
	for (int64_t i = 0; i < input_size; i++) {
		if (!visit[i].any())
			continue;
		
		D_ASSERT(info.context.csr_list[id]);
		for (auto index = (long)info.context.csr_list[id]->v[i]; index < (long)info.context.csr_list[id]->v[i + 1]; index++) {
			auto n = info.context.csr_list[id]->e[index];
			visit_next[n] = visit_next[n] | visit[i];
			
		}
		
	}
	
	for (int64_t i = 0; i < input_size ; i++) {
		if (visit_next[i].none())
			continue;
		visit_next[i] = visit_next[i] & ~seen[i];
		seen[i] = seen[i] | visit_next[i];
		if(exit_early == true && visit_next[i].any() )
			exit_early = false;
			
	}
	return exit_early;
}

static pair<bool, size_t> bfs_temp_state_variant(bool exit_early, int32_t id, int64_t input_size, MsbfsBindData &info, vector<std::bitset<LANE_LIMIT>> &seen, 
		vector<std::bitset<LANE_LIMIT>> &visit,
		vector<std::bitset<LANE_LIMIT>> &visit_next) { 
	size_t num_nodes_to_visit = 0;
	for (int64_t i = 0; i < input_size; i++) {
		if (!visit[i].any())
			continue;
		
		D_ASSERT(info.context.csr_list[id]);
		for (auto index = (long)info.context.csr_list[id]->v[i]; index < (long)info.context.csr_list[id]->v[i + 1]; index++) {
			auto n = info.context.csr_list[id]->e[index];
			visit_next[n] = visit_next[n] | visit[i];
			
		}
		
	}
	
	for (int64_t i = 0; i < input_size ; i++) {
		if (visit_next[i].none())
			continue;
		visit_next[i] = visit_next[i] & ~seen[i];
		seen[i] = seen[i] | visit_next[i];
		if(exit_early == true && visit_next[i].any() )
			exit_early = false;
		if(visit_next[i].any()){
			num_nodes_to_visit++;
		}
			
	}
	return pair<bool, size_t>(exit_early, num_nodes_to_visit);
}



static bool bfs_with_array_variant(bool exit_early, int32_t id, MsbfsBindData &info, vector<std::bitset<LANE_LIMIT>> &seen, 
		vector<std::bitset<LANE_LIMIT>> &visit,
		vector<std::bitset<LANE_LIMIT>> &visit_next, vector<int64_t> &visit_list) { 
	vector<int64_t> neighbours_list;
	for (int64_t i : visit_list) {
		if (!visit[i].any())
			continue;
		
		D_ASSERT(info.context.csr_list[id]);
		for (auto index = (long)info.context.csr_list[id]->v[i]; index < (long)info.context.csr_list[id]->v[i + 1]; index++) {
			auto n = info.context.csr_list[id]->e[index];
			visit_next[n] = visit_next[n] | visit[i];
			neighbours_list.push_back(n);
		}
	}
	visit_list.clear();
	for (int64_t i: neighbours_list) {
		if (visit_next[i].none())
			continue;
		visit_next[i] = visit_next[i] & ~seen[i];
		seen[i] = seen[i] | visit_next[i];
		if(exit_early == true && visit_next[i].any() )
			exit_early = false;
		if(visit_next[i].any()){
			visit_list.push_back(i);
		}
			
	}
	return exit_early;
}

static int find_mode(int mode, size_t visit_list_len, size_t visit_limit, size_t num_nodes_to_visit) {
	// int new_mode = 0;
	if(mode == 0 && visit_list_len > 0) {
		mode = 1;
		// new_mode = 1;
	}
	if(mode == 1 && visit_list_len > visit_limit) {
		mode = 2;
	}
	if(mode == 2 && num_nodes_to_visit < visit_limit) {
		mode = 0;
	}
	return mode;
}

// static void LogOutput(ofstream log_file, string message) {
// 	if (log_file.good()) {
// 		log_file << message << endl;
// 		log_file.flush();
// 	}
// }


static void msbfs_function(DataChunk &args, ExpressionState &state, Vector &result) {
	// D_ASSERT(args.ColumnCount() == 0);
	auto &func_expr = (BoundFunctionExpression &)state.expr;
	auto &info = (MsbfsBindData &)*func_expr.bind_info;

	int32_t id = args.data[0].GetValue(0).GetValue<int32_t>();
	bool is_variant = args.data[1].GetValue(0).GetValue<bool>();
	int64_t input_size = args.data[2].GetValue(0).GetValue<int64_t>();

	auto &src = args.data[3];

	VectorData vdata_src, vdata_target;
	src.Orrify(args.size(), vdata_src);
	
	auto src_data = (int64_t *)vdata_src.data;

	auto &target = args.data[4];
	target.Orrify(args.size(), vdata_target);
	auto target_data = (int64_t *)vdata_target.data;
	// const int32_t bfs = info.num_bfs;
	string file_name = "timings-test.txt";
	// if(args.data[5].){
		file_name = args.data[5].GetValue(0).GetValue<string>();
	// }
	idx_t result_size = 0;
	vector<int64_t> visit_list;
	size_t visit_limit = input_size / VISIT_SIZE_DIVISOR;
	size_t num_nodes_to_visit = 0;
	// idx_t curr_batch = 0;
	result.SetVectorType(VectorType::FLAT_VECTOR);
	auto result_data = FlatVector::GetData<bool>(result);
	// CycleCounter profiler;
	// FILE f1;
	ofstream log_file;
	log_file.open(info.file_name, std::ios_base::app);
	// std::stringstream ss;
	Profiler<system_clock> phase_profiler, outer_profiler;
	
	log_file << "Args size " << std::to_string(args.size()) <<endl ;
	outer_profiler.Start();
	while (result_size < args.size()) {
		// int32_t lanes = 0;
		vector<std::bitset<LANE_LIMIT>> seen(input_size);
		vector<std::bitset<LANE_LIMIT>> visit(input_size);
		vector<std::bitset<LANE_LIMIT>> visit_next(input_size);
		
		//mapping of src_value ->  (bfs_num/lane, vector of indices in src_data)
		unordered_map<int64_t, pair<int16_t, vector<int64_t>>> lane_map;
	 
	auto curr_batch_size = initialise_bfs(result_size, args.size(), src_data, vdata_src.sel,vdata_src.validity, seen, visit, visit_next, lane_map);
	
	int mode = 0;
	bool exit_early = false;
	// int i = 0;
	while (exit_early == false ) {
		// log_file << "Iter" << endl;
		exit_early =true;
		if(is_variant) {
			mode = find_mode(mode, visit_list.size(), visit_limit, num_nodes_to_visit);
			switch (mode)
			{
			case 1:
				exit_early = bfs_with_array_variant(exit_early, id, info, seen, visit, visit_next, visit_list);
				break;
			case 0:
				exit_early = bfs_without_array_variant(exit_early, id, input_size, info, seen, visit, visit_next, visit_list);
				break;
			case 2: {
				auto return_pair = bfs_temp_state_variant(exit_early, id, input_size, info, seen, visit, visit_next);
				exit_early = return_pair.first;
				num_nodes_to_visit = return_pair.second;
				break;
			}
			default:
				throw Exception("Unknown mode encountered");
			}
		}
		else {
			// profiler.BeginSample();
			phase_profiler.Start();
			exit_early = bfs_without_array(exit_early, id, input_size, info, seen, visit, visit_next);
			// profiler.EndSample(args.size());
			phase_profiler.End();
			log_file << "BFS function time " << std::to_string(phase_profiler.Elapsed()) << endl;
			// LogOutput(log_file, std::to_string(phase_profiler.Elapsed()));
			// fprintf(log_file, "%ld", ));
			
			// profiler.time;
		}
		visit = visit_next;
		for (auto i = 0; i < input_size; i++) {
			visit_next[i] = 0;
		}
		// check target
		// visit_list with n's to keep track (iniitally 64 nodes to visit but looping till input_size)
		// top down vs bottom up ->
		// adaptive with and without visit list-> benchmark
	}

	
	for (auto iter : lane_map) {
		auto value = iter.first;
		auto bfs_num = iter.second.first;
		auto pos = iter.second.second;
		for(auto index: pos) {
			auto target_index = vdata_target.sel->get_index(index);
			if(seen[target_data[target_index]][bfs_num] && seen[value][bfs_num]) {
			// if(is_bit_set(seen[target_data[index]], bfs_num) & is_bit_set(seen[value], bfs_num) ) {
				result_data[index] = true;
			}
			else
				result_data[index] = false;
		}
	}
	result_size = result_size + curr_batch_size;
	}
	outer_profiler.End();
	log_file << "Entire program time " << std::to_string(outer_profiler.Elapsed()) << endl;
	// local struct -> bitset (how many bits it contains), 
				// dynamically move between different ; can
				// 3 modes to benchmark - separate visit list ( keep track of number of nodes to visit) 
				// if nodes < count/threshold, then only use  
				// 
				// combine same source --> suboptisation 
				// allocate one lane per unique src. 
				// remember mapping of lanes to src. for loop to discover if already seen map it to that; 
				// doing sort -> not worth it ; could look into hash table. 

				// what if seen takes up too much memory ? will have to drop elements. 
				// LRU or hit count for eviction from  caching. 
				// count how often sources reoccur; first run non popular sources, then popular ones at end. 
				// immediately answer for those frequent sources. 

				// shortest path - dynamically change bit size based on distance. 
				// start with bit_size 8, if distance becomes too big 
				// copy current search state to 16 bit and resume implementation. 
				// hops using msbfs -> 
				// weights supported. 
				// 32 bit floats for storing weights --> implementations faster.  



				// interpret bi directional edges, 
				// pass flag undirected or any directed. 
				// unless explicitly given ; edge has two IDs if we support undirected edges.
				// bi directional - SQL query union all (unidirectional, duplicate edges )
				// every edge user puts in edge table is bi directional assumption ( different mode)
				// TODO: bi directional mode. 
				// two group by - one for src dense IDs, one for dest dense IDs and then do a sum of both. 
				//  
				// multiple edges across vertices can be supported. 
				// d = visit[i] & ~seen[n];
				// if (d > 0) {
				// 	visit_next[n] = visit_next[n] | d;
				// 	seen[n] = seen[n] | d;
				// }
}

static unique_ptr<FunctionData> msbfs_bind(ClientContext &context, ScalarFunction &bound_function,
                                           vector<unique_ptr<Expression>> &arguments) {
	string file_name;
	if(arguments.size() == 6) {
	// if(args.data[5].){
		file_name = ExpressionExecutor::EvaluateScalar(*arguments[5]).GetValue<string>();
		// file_name = arguments[5].GetValue(0).GetValue<string>()

	}
	else {
		file_name = "timings-test.txt";
	}
	return make_unique<MsbfsBindData>(context, file_name);
}

void MsBfsFun::RegisterFunction(BuiltinFunctions &set) {
	// params ->id, is_variant, v_size, source, target
	set.AddFunction(ScalarFunction(
	    "reachability", {LogicalType::INTEGER, LogicalType::BOOLEAN, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::BOOLEAN, msbfs_function, false, msbfs_bind));
	set.AddFunction(ScalarFunction(
	    "reach", {LogicalType::INTEGER, LogicalType::BOOLEAN, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::VARCHAR},
	    LogicalType::BOOLEAN, msbfs_function, false, msbfs_bind));
}

} // namespace duckdb