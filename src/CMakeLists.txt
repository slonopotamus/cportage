# Checks
include(CheckIncludeFile)
include(CheckFunctionExists)

check_include_file(stdbool.h HAVE_STDBOOL_H)
if(!${HAVE_STDBOOL_H})
  message(FATAL_ERROR "stdbool.h header not found")
endif()

check_include_file(sys/utsname.h HAVE_SYS_UTSNAME_H)
if(!${HAVE_SYS_UTSNAME_H})
  message(FATAL_ERROR "sys/utsname.h header not found")
endif()

check_function_exists(asprintf HAVE_ASPRINTF)
if(!${HAVE_ASPRINTF})
  message(FATAL_ERROR "asprintf function not found")
endif()

check_function_exists(getline HAVE_GETLINE)
if(!${HAVE_GETLINE})
  message(FATAL_ERROR "getline function not found")
endif()

add_subdirectory(cportage)
add_subdirectory(cmerge)
