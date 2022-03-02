/*

void ShortestPathFun::RegisterFunction(BuiltinFunctions &set) {
	// padding reqd for csr_v ; size unequal
	// w(num_bfs), v_size, source, target, wt
	set.AddFunction(ScalarFunction(
	    "shortest_path",
	    {LogicalType::INTEGER, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT, LogicalType::BIGINT},
	    LogicalType::INTEGER, sp_function, false, sp_bind));
}

} // namespace duckdb
*/