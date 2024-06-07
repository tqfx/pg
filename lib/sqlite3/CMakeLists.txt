cmake_minimum_required(VERSION 3.12)

project(sqlite3 VERSION 3.37.2 LANGUAGES C
  DESCRIPTION "SQLite is a software library that implements a self-contained, serverless, zero-configuration, transactional SQL database engine."
  HOMEPAGE_URL "https://github.com/sqlite/sqlite"
)

option(ENABLE_FTS3 "Enable the FTS3 extension" OFF)
option(ENABLE_FTS4 "Enable the FTS4 extension" OFF)
option(ENABLE_FTS5 "Enable the FTS5 extension" OFF)
option(ENABLE_MEMSYS3 "Enable MEMSYS3" OFF)
option(ENABLE_MEMSYS5 "Enable MEMSYS5" OFF)
option(ENABLE_MATH_FUNCTION "Enable math functions" OFF)
option(ENABLE_LIMIT "Enable the UPDATE/DELETE LIMIT clause" OFF)
option(ENABLE_RTREE "Enable the RTREE extension" OFF)
option(ENABLE_SESSION "Enable the SESSION extension" OFF)
option(ENABLE_OMIT_LOAD_EXT "Enable loading of external extensions" OFF)
option(WITH_GEOPOLY "Enable geopoly functionality for sqlite3" OFF)
option(WITH_JSON1 "Enable JSON functionality for sqlite3" OFF)
option(WITH_ZLIB "Build sqlite3 with zlib support" OFF)

if(BUILD_SHARED_LIBS)
  if(UNIX)
    set(SQLITE_API "-DSQLITE_API=__attribute__((visibility(\"default\")))")
  elseif(WIN32)
    set(SQLITE_API "-DSQLITE_API=__declspec(dllexport)")
  endif()
endif()

add_library(sqlite3 sqlite3.c)
add_library(unofficial::sqlite3::sqlite3 ALIAS sqlite3)

target_include_directories(sqlite3 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<INSTALL_INTERFACE:include>
)

target_compile_definitions(sqlite3 PRIVATE
  $<$<CONFIG:Debug>:SQLITE_DEBUG=1>
  $<$<CONFIG:Debug>:SQLITE_ENABLE_SELECTTRACE>
  $<$<CONFIG:Debug>:SQLITE_ENABLE_WHERETRACE>
  ${SQLITE_API}
  -DSQLITE_ENABLE_UNLOCK_NOTIFY
  -DSQLITE_ENABLE_COLUMN_METADATA
)

if(BUILD_SHARED_LIBS)
  if(UNIX)
    target_compile_definitions(sqlite3 INTERFACE "SQLITE_API=__attribute__((visibility(\"default\")))")
  elseif(WIN32)
    target_compile_definitions(sqlite3 INTERFACE "SQLITE_API=__declspec(dllimport)")
  endif()
endif()

if(ENABLE_FTS3)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_FTS3)
endif()

if(ENABLE_FTS4)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_FTS4)
endif()

if(ENABLE_FTS5)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_FTS5)
endif()

if(ENABLE_MEMSYS3)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_MEMSYS3)
endif()

if(ENABLE_MEMSYS5)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_MEMSYS5)
endif()

if(ENABLE_MATH_FUNCTION)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_MATH_FUNCTIONS)
endif()

if(ENABLE_LIMIT)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_UPDATE_DELETE_LIMIT)
endif()

if(ENABLE_RTREE)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_RTREE)
endif()

if(ENABLE_SESSION)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_SESSION SQLITE_ENABLE_PREUPDATE_HOOK)
endif()

if(ENABLE_OMIT_LOAD_EXT)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_OMIT_LOAD_EXTENSION)
endif()

if(WITH_GEOPOLY)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_GEOPOLY)
endif()

if(WITH_JSON1)
  target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_JSON1)
endif()

if(WIN32)
  target_compile_definitions(sqlite3 PUBLIC -DSQLITE_OS_WIN=1)

  if(CMAKE_SYSTEM_NAME MATCHES "WindowsStore")
    target_compile_definitions(sqlite3 PUBLIC -DSQLITE_OS_WINRT=1)
  endif()
else()
  target_compile_definitions(sqlite3 PUBLIC -DSQLITE_OS_UNIX=1)
endif()

if(NOT WIN32)
  find_package(Threads REQUIRED)
  target_link_libraries(sqlite3 PUBLIC Threads::Threads ${CMAKE_DL_LIBS})
endif()

if(WITH_ZLIB)
  find_package(ZLIB REQUIRED)
  target_link_libraries(sqlite3 PUBLIC ZLIB::ZLIB)
endif()

install(FILES sqlite3.h DESTINATION include)
install(TARGETS sqlite3
  EXPORT unofficial-sqlite3-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
install(EXPORT unofficial-sqlite3-targets
  FILE unofficial-sqlite3-targets.cmake
  NAMESPACE unofficial::sqlite3::
  DESTINATION share/unofficial-sqlite3
)
