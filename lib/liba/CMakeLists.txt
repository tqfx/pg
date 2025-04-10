set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)
cmake_minimum_required(VERSION 2.8.12...4.0)
include(cmake/CMakeModulePath.cmake)
include(cmake/Languages.cmake)
project(liba ${LANGUAGES})
include(cmake/Project.cmake)
include(cmake/core/file.cmake)
include(cmake/core/string.cmake)
include(cmake/CMakeByteOrder.cmake)
include(cmake/CMakeBuildType.cmake)
include(cmake/EnabledLanguages.cmake)
include(cmake/TargetSupportsSharedLibs.cmake)
include(cmake/TargetSupportsExecutables.cmake)
include(cmake/TargetSupportsSymbolicLink.cmake)

option(LIBA_STATIC "Enable/Disable finding and linking static libraries" 0)

if(LIBA_STATIC)
  include(cmake/CMakeFindLibraryStatic.cmake)
endif()

option(LIBA_WARNINGS "Enable/Disable warnings" 0)

if(LIBA_WARNINGS)
  include(cmake/TargetWarnings.cmake)
endif()

option(LIBA_ANALYZER "Enable/Disable analyzer" 0)

if(LIBA_ANALYZER)
  include(cmake/TargetAnalyzer.cmake)
endif()

option(LIBA_SANITIZE "Enable/Disable sanitize" 0)

if(LIBA_SANITIZE)
  include(cmake/TargetSanitize.cmake)
endif()

if(LIBA_STATIC)
  include(cmake/TargetStatic.cmake)
endif()

option(LIBA_CCACHE "Enable/Disable ccache" 0)

if(LIBA_CCACHE)
  include(cmake/UseCCache.cmake)
endif()

if(UNIX)
  set(LIBA_MATH "m" CACHE FILEPATH "system math library")
else()
  set(LIBA_MATH "" CACHE FILEPATH "system math library")
endif()

set(LIBA_INSTALL "" CACHE STRING "Specify installation behavior")
set(LIBA_REAL 8 CACHE STRING "floating-point number bytes")
set_property(CACHE LIBA_REAL PROPERTY STRINGS 4 8 16)
include(CheckLibraryExists)

function(check_math VARIABLE FUNCTION)
  get_filename_component(LOCATION "${LIBA_MATH}" DIRECTORY)
  list(APPEND CMAKE_REQUIRED_LIBRARIES ${LIBA_MATH})

  if(LIBA_REAL GREATER 8)
    set(FUNCTION ${FUNCTION}l)
  elseif(LIBA_REAL EQUAL 4)
    set(FUNCTION ${FUNCTION}f)
  endif()

  check_library_exists("${LIBA_MATH}"
    ${FUNCTION} "${LOCATION}" ${VARIABLE}
  )
endfunction()

check_math(A_HAVE_ASINH asinh)
check_math(A_HAVE_ACOSH acosh)
check_math(A_HAVE_ATANH atanh)
check_math(A_HAVE_EXPM1 expm1)
check_math(A_HAVE_LOG1P log1p)
check_math(A_HAVE_ATAN2 atan2)
check_math(A_HAVE_HYPOT hypot)
check_math(A_HAVE_CSQRT csqrt)
check_math(A_HAVE_CPOW cpow)
check_math(A_HAVE_CEXP cexp)
check_math(A_HAVE_CLOG clog)
check_math(A_HAVE_CSIN csin)
check_math(A_HAVE_CCOS ccos)
check_math(A_HAVE_CTAN ctan)
check_math(A_HAVE_CSINH csinh)
check_math(A_HAVE_CCOSH ccosh)
check_math(A_HAVE_CTANH ctanh)
check_math(A_HAVE_CASIN casin)
check_math(A_HAVE_CACOS cacos)
check_math(A_HAVE_CATAN catan)
check_math(A_HAVE_CASINH casinh)
check_math(A_HAVE_CACOSH cacosh)
check_math(A_HAVE_CATANH catanh)

option(LIBA_DOXYGEN "Enable/Disable doxygen" 0)

if(LIBA_DOXYGEN)
  find_package(Doxygen OPTIONAL_COMPONENTS dot mscgen dia)
endif()

option(LIBA_CPPCHECK "Enable/Disable cppcheck" 0)

if(LIBA_CPPCHECK)
  find_package(Cppcheck)
endif()

option(LIBA_CLANG_TIDY "Enable/Disable clang-tidy" 0)

if(LIBA_CLANG_TIDY)
  find_package(ClangTidy)
endif()

option(LIBA_IWYU "Enable/Disable include-what-you-use" 0)

if(LIBA_IWYU)
  find_package(IWYU)
endif()

option(LIBA_IPO "Enable/Disable interprocedural optimization" 0)

# https://cmake.org/cmake/help/latest/module/CheckIPOSupported.html
if(NOT CMAKE_VERSION VERSION_LESS 3.9 AND LIBA_IPO)
  cmake_policy(SET CMP0069 NEW)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT LIBA_IPO)
endif()

if(DEFINED ENV{ANDROID_ROOT} AND EXISTS $ENV{ANDROID_ROOT})
  option(LIBA_PIE "Enable/Disable position independent code" 1)
else()
  option(LIBA_PIE "Enable/Disable position independent code" 0)
endif()

# https://cmake.org/cmake/help/latest/module/CheckPIESupported.html
if(NOT CMAKE_VERSION VERSION_LESS 3.14)
  cmake_policy(SET CMP0083 NEW)
  include(CheckPIESupported)
  check_pie_supported()
endif()

function(target_msvc_no_manifest)
  foreach(target ${ARGN})
    get_property(LINK_FLAGS TARGET ${target} PROPERTY LINK_FLAGS)
    string_append(LINK_FLAGS /MANIFEST:NO)
    set_property(TARGET ${ARGN} PROPERTY LINK_FLAGS "${LINK_FLAGS}")
  endforeach()
endfunction()

function(set_library_options)
  if(LIBA_IWYU AND IWYU_FOUND)
    add_include_what_you_use(TARGETS ${ARGN})
  endif()

  if(LIBA_CLANG_TIDY AND CLANG_TIDY_FOUND)
    add_clang_tidy(TARGETS ${ARGN} OPTIONS --fix)
  endif()

  if(LIBA_CPPCHECK AND CPPCHECK_FOUND)
    add_cppcheck(TARGETS ${ARGN} OPTIONS --enable=warning,performance)
  endif()

  set_target_properties(${ARGN} PROPERTIES INTERPROCEDURAL_OPTIMIZATION ${LIBA_IPO}
    C_VISIBILITY_PRESET hidden CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN 1
  )

  if(CMAKE_C_COMPILER_ID MATCHES "TinyCC" AND CMAKE_STATIC_LIBRARY_SUFFIX STREQUAL .lib)
    set_property(TARGET ${ARGN} PROPERTY IMPORT_SUFFIX .def)
  endif()

  if(CMAKE_C_COMPILER_ID MATCHES "MSVC" AND NOT MSVC_VERSION LESS 1400)
    target_msvc_no_manifest(${ARGN})
  endif()

  if(LIBA_WARNINGS)
    target_compile_warnings(${ARGN})
  endif()

  if(LIBA_ANALYZER)
    target_compile_analyzer(${ARGN})
  endif()
endfunction()

function(add_library_properties target scope source)
  target_compile_definitions(${target} ${scope} $<TARGET_PROPERTY:${source},${ARGV3}COMPILE_DEFINITIONS>)
  target_include_directories(${target} ${scope} $<TARGET_PROPERTY:${source},${ARGV3}INCLUDE_DIRECTORIES>)

  if(${scope} MATCHES "PUBLIC|PRIVATE")
    set_property(TARGET ${target} APPEND PROPERTY LINK_LIBRARIES $<TARGET_PROPERTY:${source},${ARGV3}LINK_LIBRARIES>)
  endif()

  if(${scope} MATCHES "PUBLIC|INTERFACE")
    set_property(TARGET ${target} APPEND PROPERTY INTERFACE_LINK_LIBRARIES $<TARGET_PROPERTY:${source},${ARGV3}LINK_LIBRARIES>)
  endif()
endfunction()

option(LIBA_MIMALLOC "Enable/Disable override using mimalloc" 0)

if(LIBA_MIMALLOC)
  find_package(mimalloc)
endif()

if(mimalloc_FOUND)
  if(TARGET mimalloc-static)
    set(MIMALLOC_LIBRARY mimalloc-static)
  else()
    set(MIMALLOC_LIBRARY mimalloc)
  endif()
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/.git)
  find_package(Git)

  if(GIT_FOUND)
    function(git_command output)
      execute_process(ERROR_QUIET
        COMMAND ${GIT_EXECUTABLE} ${ARGN}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE ${output}
      )
      set(${output} ${${output}} PARENT_SCOPE)
    endfunction()

    git_command(GIT_COMMIT_LONG log -1 --pretty=format:%H)
    git_command(GIT_COMMIT_DATE log -1 --pretty=format:%cd --date=format:%Y%m%d)
  endif()

  if(GIT_COMMIT_DATE)
    set(${PROJECT_NAME}_VERSION_TWEAK ${GIT_COMMIT_DATE})
    set(PROJECT_VERSION_TWEAK ${GIT_COMMIT_DATE})
    set(A_VERSION_TWEAK ${GIT_COMMIT_DATE})
  endif()
endif()

if(NOT EXISTS "${A_HAVE_H}")
  set(A_HAVE_H ${CMAKE_CURRENT_BINARY_DIR}/a.cmake.h)
  configure_file(${CMAKE_CURRENT_LIST_DIR}/include/a.cmake.h.in ${A_HAVE_H})
endif()

if(IS_ABSOLUTE "${LIBA_MATH}")
  get_filename_component(math ${LIBA_MATH} NAME_WE)
  string(REGEX REPLACE "^${CMAKE_STATIC_LIBRARY_PREFIX}(.+)" "\\1" math "${math}")
  set(LIBA_MATH $<BUILD_INTERFACE:${LIBA_MATH}> $<INSTALL_INTERFACE:${math}>)
endif()

if(TARGET_SUPPORTS_SYMBOLIC_LINK)
  option(LIBA_SYMLINK "Enable/Disable symbolic link" 1)
endif()

function(set_library_compile target)
  if(EXISTS "${A_HAVE_H}")
    file(RELATIVE_PATH a_have_h ${PROJECT_SOURCE_DIR}/include/a ${A_HAVE_H})
    target_compile_definitions(${target} PUBLIC $<BUILD_INTERFACE:A_HAVE_H="${a_have_h}">)
  endif()

  if((LIBA_SYMLINK OR WIN32) AND TARGET_SUPPORTS_SYMBOLIC_LINK)
    set_target_properties(${target} PROPERTIES SOVERSION
      ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION}
    )
  endif()

  set_target_properties(${target} PROPERTIES OUTPUT_NAME a
    POSITION_INDEPENDENT_CODE 1 DEFINE_SYMBOL A_EXPORTS
  )
  target_compile_definitions(${target} PRIVATE A_EXPORTS)
  target_include_directories(${target} BEFORE PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
  get_property(TYPE TARGET ${target} PROPERTY TYPE)

  if(${TYPE} MATCHES "(STATIC|SHARED|MODULE)_LIBRARY")
    target_link_libraries(${target} PUBLIC ${LIBA_MATH})
  endif()

  set_library_options(${target})
endfunction()

if(LIBA_CXX)
  file_scaner(SOURCES RECURSE include src EXT c h cc hh cpp hpp)
else()
  file_scaner(SOURCES RECURSE include src EXT c h)
endif()

add_library(a OBJECT ${SOURCES})
set_library_compile(a)

if(LIBA_MIMALLOC AND mimalloc_FOUND)
  set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/src/a.c
    APPEND PROPERTY COMPILE_DEFINITIONS A_HAVE_MIMALLOC_H
  )

  if(CMAKE_VERSION VERSION_LESS 3.12)
    add_library_properties(a PUBLIC ${MIMALLOC_LIBRARY} INTERFACE_)
  else()
    target_link_libraries(a PUBLIC ${MIMALLOC_LIBRARY})
  endif()
endif()

include(GNUInstallDirs)
set(INSTALL_TARGETS "")

if("${LIBA_INSTALL}" MATCHES "^$|static")
  list(APPEND INSTALL_TARGETS alib)
endif()

add_library(alib STATIC $<TARGET_OBJECTS:a>)
set_library_compile(alib)

if(LIBA_MIMALLOC AND mimalloc_FOUND)
  target_link_libraries(alib PUBLIC ${MIMALLOC_LIBRARY})
endif()

if(TARGET_SUPPORTS_SHARED_LIBS)
  if("${LIBA_INSTALL}" MATCHES "^$|shared")
    list(APPEND INSTALL_TARGETS liba)
  endif()

  add_library(liba SHARED $<TARGET_OBJECTS:a>)
  set_library_compile(liba)

  if(LIBA_MIMALLOC AND mimalloc_FOUND)
    target_link_libraries(liba PUBLIC ${MIMALLOC_LIBRARY})
  endif()

  if(CMAKE_STATIC_LIBRARY_SUFFIX STREQUAL .lib)
    set_property(TARGET liba PROPERTY OUTPUT_NAME liba)
  endif()

  target_compile_definitions(liba INTERFACE A_IMPORTS)

  if(LIBA_STATIC)
    target_link_static(liba)
  endif()
endif()

if(LIBA_SANITIZE)
  add_library(asan OBJECT ${SOURCES})
  set_library_compile(asan)
  target_compile_sanitize(asan)

  if(LIBA_MIMALLOC AND mimalloc_FOUND)
    if(CMAKE_VERSION VERSION_LESS 3.12)
      add_library_properties(asan PUBLIC ${MIMALLOC_LIBRARY} INTERFACE_)
    else()
      target_link_libraries(asan PUBLIC ${MIMALLOC_LIBRARY})
    endif()
  endif()
endif()

if(INSTALL_TARGETS)
  include(cmake/InstallTargets.cmake)
  install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/a DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  if(EXISTS "${A_HAVE_H}")
    install(FILES ${A_HAVE_H} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/a)
    install_hook(${CMAKE_INSTALL_INCLUDEDIR}/a/a.h A_HAVE_H ${A_HAVE_H})
  endif()
endif()

option(LIBA_PKGCONFIG "Enable/Disable pkgconfig" 0)

if(LIBA_PKGCONFIG)
  include(cmake/InstallPkgConfig.cmake)
  install_pkgconfig(${PROJECT_NAME} alib)
endif()

option(LIBA_LIBTOOL "Enable/Disable libtool" 0)

if(LIBA_LIBTOOL AND INSTALL_TARGETS)
  include(cmake/InstallLibtool.cmake)
  install_libtool(${INSTALL_TARGETS})
endif()

set(LIBA_VCPKG "" CACHE PATH "vcpkg installation root")

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/vcpkg AND LIBA_VCPKG AND GIT_COMMIT_LONG)
  if(NOT EXISTS ${LIBA_VCPKG})
    set(LIBA_VCPKG ${CMAKE_CURRENT_BINARY_DIR}/vcpkg)
    install(DIRECTORY ${LIBA_VCPKG} DESTINATION ${CMAKE_INSTALL_DATADIR})
  endif()

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/portfile.cmake
    ${LIBA_VCPKG}/ports/${PROJECT_NAME}/portfile.cmake
    @ONLY
  )
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg.json
    ${LIBA_VCPKG}/ports/${PROJECT_NAME}/vcpkg.json
    @ONLY
  )
endif()

if(PROJECT_IS_TOP_LEVEL)
  add_custom_target(afmt)
  add_custom_target(adoc)
  include(CTest)
endif()

if(NOT CMAKE_FIND_LIBRARY_PREFIXES)
  set(CMAKE_FIND_LIBRARY_PREFIXES ";lib")
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/java/CMakeLists.txt)
  option(LIBA_JAVA "Enable/Disable java" 0)

  if(LIBA_JAVA)
    find_package(Java)
    find_package(JNI)
  endif()

  if(LIBA_JAVA AND Java_JAVAC_EXECUTABLE)
    add_subdirectory(java)
  endif()
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/javascript/CMakeLists.txt)
  if(EMSCRIPTEN)
    option(LIBA_JAVASCRIPT "Enable/Disable javascript" 1)
  else()
    option(LIBA_JAVASCRIPT "Enable/Disable javascript" 0)
  endif()

  if(LIBA_JAVASCRIPT)
    if(NOT EMSCRIPTEN)
      find_package(Emscripten)
    endif()

    if(EMSCRIPTEN OR EMSCRIPTEN_FOUND)
      add_subdirectory(javascript)
    endif()
  endif()
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/lua/CMakeLists.txt)
  option(LIBA_LUA "Enable/Disable lua" 0)
  option(LIBA_LUAJIT "Enable/Disable luajit" 0)
  option(LIBA_WITH_LUA "Enable/Disable build with lua" 0)

  if(LIBA_LUA OR LIBA_LUAJIT OR LIBA_WITH_LUA)
    set(LUA_MATH_LIBRARY ${LIBA_MATH} CACHE STRING "lua math library")
  endif()

  if(NOT LIBA_WITH_LUA AND LIBA_LUA)
    find_package(Lua OPTIONAL_COMPONENTS lua)
    set(LUA_LIBRARIES lualib)
  endif()

  if((LIBA_LUA AND NOT LUA_FOUND AND NOT LIBA_WITH_LUA) OR LIBA_LUAJIT)
    find_package(LuaJIT OPTIONAL_COMPONENTS luajit)
    set(LUA_EXECUTABLE ${LUAJIT_EXECUTABLE})
    set(LUA_LIBRARIES luajit)
  endif()

  if(LIBA_LUA AND LUA_FOUND)
    add_subdirectory(lua)
  elseif(LIBA_LUAJIT AND LUAJIT_FOUND)
    add_subdirectory(lua)
  elseif(LIBA_WITH_LUA)
    add_subdirectory(lua)
  endif()
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/python/CMakeLists.txt)
  option(LIBA_PYTHON "Enable/Disable python" 0)

  if(LIBA_PYTHON)
    find_package(Python COMPONENTS Development OPTIONAL_COMPONENTS Interpreter)
    find_package(Cython)
  endif()

  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/python/src/liba.c OR CYTHON_FOUND)
    if(Python_FOUND)
      add_subdirectory(python)
    endif()
  endif()
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/quickjs/CMakeLists.txt)
  option(LIBA_QUICKJS "Enable/Disable quickjs" 0)
  option(LIBA_WITH_QUICKJS "Enable/Disable build with quickjs" 0)

  if(LIBA_QUICKJS OR LIBA_WITH_QUICKJS)
    set(QUICKJS_MATH_LIBRARY ${LIBA_MATH} CACHE STRING "quickjs math library")
  endif()

  if(NOT LIBA_WITH_QUICKJS AND LIBA_QUICKJS)
    find_package(QuickJS)
  endif()

  if(LIBA_QUICKJS AND QUICKJS_FOUND)
    add_subdirectory(quickjs)
  elseif(LIBA_WITH_QUICKJS)
    add_subdirectory(quickjs)
  endif()
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt)
  if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    add_subdirectory(test)
  endif()
endif()

set(CPACK_SOURCE_IGNORE_FILES ${CMAKE_CURRENT_LIST_DIR}/.git)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/.git AND GIT_FOUND)
  execute_process(ERROR_QUIET
    COMMAND ${GIT_EXECUTABLE} status -z --ignored=matching
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE ignore
  )
  string(REPLACE "." "\\\\." ignore "${ignore}")
  string(REPLACE "*" ".*" ignore "${ignore}")
  string(REPLACE "?? " "!! " ignore "${ignore}")
  string(REGEX MATCHALL "!! [^!]+" ignore "${ignore}")
  string(REPLACE "!! " "${CMAKE_CURRENT_LIST_DIR}/" ignore "${ignore}")
  list(APPEND CPACK_SOURCE_IGNORE_FILES ${ignore})
endif()

set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_LIST_DIR}/README.md)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_LIST_DIR}/LICENSE.txt)
include(CPack)

if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
  find_package(ClangFormat)
endif()

if(CLANG_FORMAT_FOUND)
  file_scaner(SOURCES EXT c h cc hh cpp hpp RECURSE include src test
    java/src javascript/src lua/src quickjs/src
  )
  add_clang_format(afmt.c VERBOSE ${SOURCES})
  add_dependencies(afmt afmt.c)
endif()

if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
  find_package(Black)
endif()

if(BLACK_FOUND)
  add_black(afmt.py ${CMAKE_CURRENT_LIST_DIR})
  add_dependencies(afmt afmt.py)
endif()

if(NOT CMAKE_VERSION VERSION_LESS 3.9 AND DOXYGEN_FOUND)
  # Project related configuration options
  set(DOXYGEN_CREATE_SUBDIRS YES)
  set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)

  # Build related configuration options
  set(DOXYGEN_EXTRACT_STATIC YES)

  # Configuration options related to warning and progress messages
  set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
  set(DOXYGEN_WARN_NO_PARAMDOC YES)

  # Configuration options related to the input files
  set(DOXYGEN_RECURSIVE YES)

  # Configuration options related to source browsing
  # Configuration options related to the alphabetical class index
  # Configuration options related to the HTML output
  set(DOXYGEN_GENERATE_HTML YES)
  set(DOXYGEN_USE_MATHJAX YES)
  set(DOXYGEN_MATHJAX_VERSION MathJax_3)

  # Configuration options related to the LaTeX output
  # Configuration options related to the RTF output
  # Configuration options related to the man page output
  # Configuration options related to the XML output
  set(DOXYGEN_GENERATE_XML YES)

  # Configuration options related to the DOCBOOK output
  # Configuration options for the AutoGen Definitions output
  # Configuration options related to Sqlite3 output
  # Configuration options related to the Perl module output
  # Configuration options related to the preprocessor
  set(DOXYGEN_MACRO_EXPANSION YES)
  set(DOXYGEN_PREDEFINED
    "__STDC_VERSION__=201112L"
    "__cplusplus=201103L"
    "A_INLINE:=inline"
    "A_NONULL():="
    "A_FORMAT():="
    "A_EXTERN:="
    "A_INTERN:="
    "A_PUBLIC:="
    "A_HIDDEN:="
  )

  file(STRINGS "${A_HAVE_H}" macros REGEX "#define[ ]+A_HAVE_.+")

  foreach(macro ${macros})
    string(REGEX REPLACE "#define[ ]+(.*)[ ]+(.+)" "\\1=\\2" macro "${macro}")
    list(APPEND DOXYGEN_PREDEFINED ${macro})
  endforeach()

  # Configuration options related to external references
  # Configuration options related to the dot tool
  if(Doxygen_dot_FOUND)
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_INTERACTIVE_SVG YES)
    set(DOXYGEN_DOT_IMAGE_FORMAT svg)
    set(DOXYGEN_DOT_MULTI_TARGETS YES)
  endif()

  # https://www.doxygen.nl/manual/config.html
  doxygen_add_docs(adoc.c WORKING_DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/include/a
    ${CMAKE_CURRENT_LIST_DIR}/include/a
    ${CMAKE_CURRENT_LIST_DIR}/README.md
  )
  add_dependencies(adoc adoc.c)
endif()
