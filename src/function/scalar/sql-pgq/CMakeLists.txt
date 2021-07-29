add_library_unity(duckdb_func_sql_pgq OBJECT create_csr.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:duckdb_func_sql_pgq>
    PARENT_SCOPE)
