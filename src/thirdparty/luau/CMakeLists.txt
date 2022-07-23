include(FetchContent)
FetchContent_Declare(
    luau
    GIT_REPOSITORY https://github.com/Roblox/luau.git
    GIT_TAG 0.536
)
FetchContent_MakeAvailable(luau)
