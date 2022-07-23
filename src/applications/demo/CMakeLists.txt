set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-* -extra-arg=-std=c++17)
add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE a_luau)
