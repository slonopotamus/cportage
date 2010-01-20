# Dependencies
find_package(PCRE REQUIRED)
find_package(Popt REQUIRED)

# Checks
include(CheckIncludeFile)
include(CheckFunctionExists)

macro(require_include_file _include_filename)
  check_include_file(${_include_filename} _have_include_filename)
  if(!${_have_include_filename})
    message(FATAL_ERROR "${_have_include_filename} header not found")
  endif()
endmacro()

macro(require_function _function_name)
  check_function_exists(${_function_name} _function_name)
  if(!${_function_name})
    message(FATAL_ERROR "${_function_name} function not found")
  endif()
endmacro()

require_include_file(libiberty.h)
require_include_file(stdbool.h)
require_include_file(sys/utsname.h)
require_function(asprintf)
require_function(getline)

add_subdirectory(cportage)
add_subdirectory(cmerge)
