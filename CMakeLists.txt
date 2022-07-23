cmake_minimum_required(VERSION 3.2)

project(game-engine)

enable_testing()

#set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-* -extra-arg=-std=c++17)
#find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
#set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,modernize-* -extra-arg=-std=c++17")

add_subdirectory(src)
