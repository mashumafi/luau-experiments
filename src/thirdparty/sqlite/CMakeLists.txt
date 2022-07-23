include(FetchContent)

FetchContent_Declare(
  sqlite
  URL      https://sqlite.org/2022/sqlite-amalgamation-3390200.zip
  URL_HASH SHA3_256=deb2abef617b6305525e3b1a2b39a5dc095ffb62f243b4d1b468ba5f41900ce7
)

FetchContent_GetProperties(sqlite)
if(NOT sqlite_POPULATED)
    FetchContent_Populate(sqlite)
    add_library(sqlite ${sqlite_SOURCE_DIR}/sqlite3.c)
    target_include_directories(sqlite PUBLIC ${sqlite_SOURCE_DIR})
endif()
