cmake_minimum_required(VERSION 3.10)
project(FlecCompiler)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_STANDARD 17)

add_executable(flec
    main.cpp
    parser.cpp
    lexer.cpp
    ast.cpp
    codegen.cpp
    SymbolTable.cpp
)

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(flec ${llvm_libs})
