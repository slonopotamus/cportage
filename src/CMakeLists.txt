# Checks
include(CheckIncludeFile)
include(CheckFunctionExists)

check_include_file(stdbool.h HAVE_STDBOOL_H)
if(!${HAVE_STDBOOL_H})
	message(FATAL_ERROR "stdbool.h header not found")
endif()

check_function_exists(asprintf HAVE_ASPRINTF)
if(!${HAVE_ASPRINTF})
	message(FATAL_ERROR "asprintf function is required")
endif()

add_subdirectory(cportage)
add_subdirectory(cmerge)
