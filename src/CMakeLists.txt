# Dependencies
find_package(PCRE REQUIRED)
find_package(Popt REQUIRED)

# Checks
set(CMAKE_REQUIRED_FLAGS "-std=c99 -Wall -Wextra -pedantic -fvisibility=hidden")
set(CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE -D_FORTIFY_SOURCE=2")

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

# Executables
include_directories("${PROJECT_SOURCE_DIR}/include")
add_definitions(${CMAKE_REQUIRED_FLAGS} ${CMAKE_REQUIRED_DEFINITIONS})
add_library(cportage SHARED atom.c config.c object.c)
target_link_libraries(cportage ${PCRE_LIBRARY})
add_executable(cmerge cmerge.c)
target_link_libraries(cmerge cportage ${Popt_LIBRARY})

# Tests
enable_testing()

# Installation
