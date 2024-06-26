set(CMAKE_EXPORT_COMPILE_COMMANDS 1 CACHE BOOL "Enable/Disable output of compile commands during generation.")
mark_as_advanced(CMAKE_EXPORT_COMPILE_COMMANDS)
if(NOT DEFINED CACHE{LIBA_CXX})
  option(LIBA_CXX "Enable/Disable C++" 1)
endif()
list(FIND LANGUAGES CXX found)
if(${found} EQUAL -1 AND LIBA_CXX)
  list(INSERT LANGUAGES 0 CXX)
endif()
list(FIND LANGUAGES C found)
if(${found} EQUAL -1)
  list(INSERT LANGUAGES 0 C)
endif()
# default C standard
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()
if(NOT CMAKE_C_EXTENSIONS)
  set(CMAKE_C_EXTENSIONS 0)
endif()
if(NOT CMAKE_C_STANDARD_REQUIRED)
  set(CMAKE_C_STANDARD_REQUIRED 0)
endif()
# default C++ standard
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()
if(NOT CMAKE_CXX_EXTENSIONS)
  set(CMAKE_CXX_EXTENSIONS 0)
endif()
if(NOT CMAKE_CXX_STANDARD_REQUIRED)
  set(CMAKE_CXX_STANDARD_REQUIRED 0)
endif()
