set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-* -extra-arg=-std=c++17)

add_library(a_luau
    a_luau_state.cpp
)
target_link_libraries(a_luau PUBLIC Luau.Compiler Luau.VM)
target_include_directories(a_luau PUBLIC .)
target_compile_features(a_luau PRIVATE cxx_std_17)

add_executable(a_luau_test
    a_luau_state.t.cpp
)
target_link_libraries(a_luau_test PRIVATE a_luau a_catch)
add_test(a_luau_test a_luau_test)
