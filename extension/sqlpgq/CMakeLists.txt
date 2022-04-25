cmake_minimum_required(VERSION 2.8.12)

project(SQLPGQExtension)

include_directories(include)
#include_directories(vectorclass)
add_subdirectory(sqlpgq_functions)
#add_subdirectory(vectorclass)


set(SQLPGQ_EXTENSION_FILES
    sqlpgq-extension.cpp
    sqlpgq_functions/sqlpgq_cheapest_path.cpp
    sqlpgq_functions/sqlpgq_shortest_path.cpp
    sqlpgq_functions/sqlpgq_csr_creation.cpp
#        ${VECTORCLASS_OBJECT_FILES}
    )

add_library(sqlpgq_extension STATIC ${SQLPGQ_EXTENSION_FILES})

build_loadable_extension(sqlpgq ${SQLPGQ_EXTENSION_FILES})

install(
    TARGETS sqlpgq_extension
    EXPORT "${DUCKDB_EXPORT_SET}"
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")