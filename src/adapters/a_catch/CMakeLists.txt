add_library(a_catch catch2.m.cpp)
target_link_libraries(a_catch PUBLIC Catch2WithMain)
target_compile_features(a_catch PRIVATE cxx_std_17)
