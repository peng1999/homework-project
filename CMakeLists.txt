cmake_minimum_required(VERSION 3.12)
#set(CMAKE_CXX_STANDARD 11)
project(Lang)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_package(BISON)
find_package(FLEX)

bison_target(Parser lang.ypp ${CMAKE_CURRENT_BINARY_DIR}/lang.tab.cpp)
flex_target(Scanner lang.l ${CMAKE_CURRENT_BINARY_DIR}/lang.yy.cpp)
add_flex_bison_dependency(Scanner Parser)

add_executable(lang
        ast.cpp ast.h
        ${BISON_Parser_OUTPUTS}
        ${FLEX_Scanner_OUTPUTS}
        intern.cpp intern.h parse.cpp parse.h)

target_include_directories(lang PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(lang PRIVATE ".")
target_compile_features(lang PRIVATE cxx_std_11)
