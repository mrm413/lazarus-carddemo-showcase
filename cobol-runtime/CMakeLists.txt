cmake_minimum_required(VERSION 3.16)

# SQLite (C library)
add_library(sqlite3 STATIC
    vendor/sqlite3/sqlite3.c
)
target_include_directories(sqlite3 PUBLIC vendor/sqlite3)
if(UNIX)
    target_link_libraries(sqlite3 PUBLIC pthread dl)
endif()

# COBOL Runtime (static library)
add_library(cobol-runtime STATIC
    src/vsam_store.cpp
    src/bms_registry.cpp
    src/cics_context.cpp
    src/sql_context.cpp
    src/transaction_loop.cpp
    src/web_server.cpp
)

target_include_directories(cobol-runtime PUBLIC
    include
    vendor/sqlite3
    vendor/httplib
)

target_link_libraries(cobol-runtime PUBLIC sqlite3)

if(WIN32)
    target_compile_definitions(cobol-runtime PUBLIC _WIN32_WINNT=0x0A00)
    target_link_libraries(cobol-runtime PUBLIC ws2_32)
endif()

# Tests
option(BUILD_TESTS "Build test suite" ON)
if(BUILD_TESTS)
    enable_testing()

    add_executable(cobol-runtime-tests
        tests/test_main.cpp
        tests/test_fixed_string.cpp
        tests/test_decimal.cpp
        tests/test_safe_int.cpp
        tests/test_vsam.cpp
        tests/test_bms.cpp
        tests/test_cics.cpp
        tests/test_sql.cpp
        tests/test_transaction.cpp
        tests/test_integration.cpp
    )

    target_include_directories(cobol-runtime-tests PRIVATE
        include
        vendor/doctest
    )

    target_link_libraries(cobol-runtime-tests PRIVATE cobol-runtime)

    add_test(NAME runtime-tests COMMAND cobol-runtime-tests)
endif()
