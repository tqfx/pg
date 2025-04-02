cmake_minimum_required(VERSION 3.15)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Specifies the build type on single-configuration generators." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project(pg VERSION 0.1.0 LANGUAGES C
  DESCRIPTION "A password generator"
  HOMEPAGE_URL "https://github.com/tqfx/pg"
)

option(PG_WARNINGS "Enable/Disable warnings" 0)

if(PG_WARNINGS)
  include(${CMAKE_CURRENT_LIST_DIR}/lib/liba/cmake/TargetWarnings.cmake)
  option(LIBA_WARNINGS "Enable/Disable warnings" 1)
endif()

option(PG_SANITIZE "Enable/Disable sanitize" 0)

if(PG_SANITIZE)
  include(${CMAKE_CURRENT_LIST_DIR}/lib/liba/cmake/TargetSanitize.cmake)
  option(LIBA_SANITIZE "Enable/Disable sanitize" 1)
endif()

include(CheckIncludeFiles)

check_include_files(getopt.h HAVE_GETOPT_H)
check_include_files(sqlite3.h HAVE_SQLITE3_H)
check_include_files(cjson/cJSON.h HAVE_CJSON_H)

file(GLOB SOURCES include/pg/*.h src/*.[chi])
add_library(pg ${SOURCES})
set_target_properties(pg PROPERTIES DEFINE_SYMBOL PG_EXPORTS
  POSITION_INDEPENDENT_CODE 1 C_VISIBILITY_PRESET hidden
)
target_compile_definitions(pg PRIVATE PG_EXPORTS)
target_include_directories(pg PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/lib>
  $<INSTALL_INTERFACE:include>
)

if(NOT HAVE_SQLITE3_H)
  add_subdirectory(lib/sqlite3 EXCLUDE_FROM_ALL)
endif()

if(NOT HAVE_CJSON_H)
  add_subdirectory(lib/cjson EXCLUDE_FROM_ALL)
endif()

target_link_libraries(pg PUBLIC sqlite3 cjson)

add_subdirectory(lib/liba EXCLUDE_FROM_ALL)

if(PG_WARNINGS)
  if(CMAKE_C_COMPILER_ID MATCHES "[Cc]lang|LLVM" OR CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang|LLVM")
    warnings_flag_cc(-Wno-declaration-after-statement)
  endif()

  target_compile_warnings(pg)
endif()

if(PG_SANITIZE)
  target_link_libraries(pg PUBLIC asan)
else()
  target_link_libraries(pg PUBLIC alib)
endif()

if(PG_SANITIZE)
  target_compile_sanitize(pg)
  target_link_sanitize(pg)
endif()

file(GLOB_RECURSE SOURCES src/cli/*.[ch])
add_executable(pg-cli ${SOURCES})
set_target_properties(pg-cli PROPERTIES RUNTIME_OUTPUT_NAME pg)
target_link_libraries(pg-cli PRIVATE pg)

if(NOT HAVE_GETOPT_H)
  target_include_directories(pg-cli PRIVATE lib/getopt)
  target_sources(pg-cli PRIVATE lib/getopt/getopt.c)
endif()

if(PG_WARNINGS)
  target_compile_warnings(pg-cli)
endif()

if(PG_SANITIZE)
  target_compile_sanitize(pg-cli)
  target_link_sanitize(pg-cli)
endif()

include(GNUInstallDirs)
install(TARGETS pg-cli
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)

if(CURSES_FOUND)
  file(GLOB_RECURSE SOURCES src/tui/*.[ch])
  add_executable(pg-tui ${SOURCES})
  target_link_libraries(pg-tui PRIVATE pg ${CURSES_LIBRARIES})
  target_include_directories(pg-tui PRIVATE ${CURSES_INCLUDE_DIRS})
  target_compile_definitions(pg-tui PRIVATE NCURSES_STATIC)

  if(NOT HAVE_GETOPT_H)
    target_include_directories(pg-tui PRIVATE lib/getopt)
    target_sources(pg-tui PRIVATE lib/getopt/getopt.c)
  endif()

  if(PG_WARNINGS)
    target_compile_warnings(pg-tui)
  endif()

  if(PG_SANITIZE)
    target_compile_sanitize(pg-tui)
    target_link_sanitize(pg-tui)
  endif()

  install(TARGETS pg-tui
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
endif()
