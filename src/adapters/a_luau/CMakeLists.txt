add_library(a_luau
    a_luau_state.cpp
)
target_link_libraries(a_luau PUBLIC Luau.Compiler Luau.VM)
target_include_directories(a_luau PUBLIC .)
