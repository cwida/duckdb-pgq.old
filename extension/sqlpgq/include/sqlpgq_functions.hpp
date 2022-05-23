//===----------------------------------------------------------------------===//
//                         DuckDB
//
// sqlpgq_functions.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/parser/parsed_data/create_scalar_function_info.hpp"

namespace duckdb {

#define LANE_LIMIT         1024
#define VISIT_SIZE_DIVISOR 2


class SQLPGQFunctions {
public:
	static vector<CreateScalarFunctionInfo> GetFunctions() {
		vector<CreateScalarFunctionInfo> functions;

		// Extract functions
//		AddAliases({"json_extract", "json_extract_path"}, GetExtractFunction(), functions);
//		AddAliases({"json_extract_string", "json_extract_path_text", "->>"}, GetExtractStringFunction(), functions);

		// Create functions
		functions.push_back(GetCsrFunction());
		functions.push_back(GetShortestPathFunction());
		functions.push_back(GetCheapestPathFunction());
		functions.push_back(GetAnyShortestPathFunction());
		functions.push_back(GetReachabilityFunction());
		functions.push_back(GetDeleteCsrFunction());
//		AddAliases({"to_json", "json_quote"}, GetToJSONFunction(), functions);
//		functions.push_back(GetArrayToJSONFunction());
//		functions.push_back(GetRowToJSONFunction());

		// Structure/Transform
//		functions.push_back(GetStructureFunction());
//		AddAliases({"json_transform", "from_json"}, GetTransformFunction(), functions);
//		AddAliases({"json_transform_strict", "from_json_strict"}, GetTransformStrictFunction(), functions);

		// Other
//		functions.push_back(GetArrayLengthFunction());
//		functions.push_back(GetTypeFunction());
//		functions.push_back(GetValidFunction());

		return functions;
	}

private:
	static CreateScalarFunctionInfo GetCsrFunction();
	static CreateScalarFunctionInfo GetShortestPathFunction();
	static CreateScalarFunctionInfo GetCheapestPathFunction();
	static CreateScalarFunctionInfo GetAnyShortestPathFunction();
	static CreateScalarFunctionInfo GetReachabilityFunction();
	static CreateScalarFunctionInfo GetDeleteCsrFunction();
//	static CreateScalarFunctionInfo GetRowToJSONFunction();
//
//	static CreateScalarFunctionInfo GetStructureFunction();
//	static CreateScalarFunctionInfo GetTransformFunction();
//	static CreateScalarFunctionInfo GetTransformStrictFunction();
//
//	static CreateScalarFunctionInfo GetArrayLengthFunction();
//	static CreateScalarFunctionInfo GetTypeFunction();
//	static CreateScalarFunctionInfo GetValidFunction();

	static void AddAliases(vector<string> names, CreateScalarFunctionInfo fun,
	                       vector<CreateScalarFunctionInfo> &functions) {
		for (auto &name : names) {
			fun.name = name;
			functions.push_back(fun);
		}
	}
};

} // namespace duckdb