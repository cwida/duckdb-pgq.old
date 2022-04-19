add_library_unity(
    duckdb_sqlpgq_functions
    OBJECT
    sqlpgq_csr_creation.cpp
    sqlpgq_shortest_path.cpp
    sqlpgq_cheapest_path.cpp
)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:duckdb_sqlpgq_functions>
    PARENT_SCOPE)