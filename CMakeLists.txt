cmake_minimum_required(VERSION 3.16)
project(carddemo VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Runtime library
add_subdirectory(cobol-runtime)

# CardDemo executable (all 44 programs + registry + main)
# NOTE: Programs need per-file namespaces to resolve duplicate symbols.
# Build with -DBUILD_CARDDEMO=ON after namespace wrapping is complete.
option(BUILD_CARDDEMO "Build the CardDemo executable" ON)
if(BUILD_CARDDEMO)
add_executable(carddemo
    programs/main.cpp
    programs/program_registry.cpp
    programs/CBACT01C_hardened.cpp
    programs/CBACT02C_hardened.cpp
    programs/CBACT03C_hardened.cpp
    programs/CBACT04C_hardened.cpp
    programs/CBCUS01C_hardened.cpp
    programs/CBEXPORT_hardened.cpp
    programs/CBIMPORT_hardened.cpp
    programs/CBPAUP0C_hardened.cpp
    programs/CBSTM03A_hardened.cpp
    programs/CBSTM03B_hardened.cpp
    programs/CBTRN01C_hardened.cpp
    programs/CBTRN02C_hardened.cpp
    programs/CBTRN03C_hardened.cpp
    programs/COACCT01_hardened.cpp
    programs/COACTUPC_hardened.cpp
    programs/COACTVWC_hardened.cpp
    programs/COADM01C_hardened.cpp
    programs/COBIL00C_hardened.cpp
    programs/COBSWAIT_hardened.cpp
    programs/COBTUPDT_hardened.cpp
    programs/COCRDLIC_hardened.cpp
    programs/COCRDSLC_hardened.cpp
    programs/COCRDUPC_hardened.cpp
    programs/CODATE01_hardened.cpp
    programs/COMEN01C_hardened.cpp
    programs/COPAUA0C_hardened.cpp
    programs/COPAUS0C_hardened.cpp
    programs/COPAUS1C_hardened.cpp
    programs/COPAUS2C_hardened.cpp
    programs/CORPT00C_hardened.cpp
    programs/COSGN00C_hardened.cpp
    programs/COTRN00C_hardened.cpp
    programs/COTRN01C_hardened.cpp
    programs/COTRN02C_hardened.cpp
    programs/COTRTLIC_hardened.cpp
    programs/COTRTUPC_hardened.cpp
    programs/COUSR00C_hardened.cpp
    programs/COUSR01C_hardened.cpp
    programs/COUSR02C_hardened.cpp
    programs/COUSR03C_hardened.cpp
    programs/CSUTLDTC_hardened.cpp
    programs/DBUNLDGS_hardened.cpp
    programs/PAUDBLOD_hardened.cpp
    programs/PAUDBUNL_hardened.cpp
)
target_include_directories(carddemo PRIVATE
    cobol-runtime/include
    programs
)
target_link_libraries(carddemo PRIVATE cobol-runtime)
endif()
