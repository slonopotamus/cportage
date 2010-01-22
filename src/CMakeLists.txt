# Dependencies
find_package(PCRE REQUIRED)
find_package(POPT REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Checks

include(CheckIncludeFile)

check_include_file(libiberty.h HAVE_LIBIBERTY_H)
if (!${HAVE_LIBIBERTY_H})
    message(FATAL_ERROR "libiberty.h is required")
endif()

check_include_file(stdbool.h HAVE_STDBOOL_H)
if (!${HAVE_STDBOOL_H})
    message(FATAL_ERROR "stdbool.h is required")
endif()

check_include_file(sys/utsname.h HAVE_SYS_UTSNAME_H)
if (!${HAVE_SYS_UTSNAME_H})
    message(FATAL_ERROR "sys/utsname.h is required")
endif()

include(CheckFunctionExists)

check_function_exists(asprintf HAVE_ASPRINTF)
if(!${HAVE_ASPRINTF})
  message(FATAL_ERROR "asprintf is required")
endif()

check_function_exists(getline HAVE_GETLINE)
if(!${HAVE_GETLINE})
  message(FATAL_ERROR "getline is required")
endif()

include(CheckLibraryExists)

check_library_exists(iberty xmalloc "" HAVE_LIBIBERTY)
if(!HAVE_LIBIBERTY)
  message(FATAL_ERROR "libiberty not found")
endif()

add_subdirectory(cportage)
add_subdirectory(cmerge)
