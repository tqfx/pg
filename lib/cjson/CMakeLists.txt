cmake_minimum_required(VERSION 3.12)

project(cjson VERSION 1.7.18 LANGUAGES C
  DESCRIPTION "Ultralightweight JSON parser in ANSI C"
  HOMEPAGE_URL "https://github.com/DaveGamble/cJSON"
)

add_library(cjson cJSON.c)

target_include_directories(cjson PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(cjson PROPERTIES C_VISIBILITY_PRESET hidden)
target_compile_definitions(cjson PUBLIC CJSON_EXPORT_SYMBOLS CJSON_API_VISIBILITY)

install(FILES cJSON.h DESTINATION include)
install(TARGETS cjson
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
install(EXPORT ${PROJECT_NAME}-targets
  FILE ${PROJECT_NAME}-targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)
