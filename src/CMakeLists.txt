# Dependencies
find_package(PCRE REQUIRED)
find_package(Popt REQUIRED)

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

# Executables
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_library(cportage SHARED atom.c settings.c object.c)
target_link_libraries(cportage ${PCRE_LIBRARY})
add_executable(cmerge cmerge.c)
target_link_libraries(cmerge cportage ${Popt_LIBRARY})

# Installation
install(TARGETS
	cportage
	LIBRARY DESTINATION lib)
install(TARGETS
	cmerge
	RUNTIME DESTINATION bin)