# Dependencies
find_package(Popt REQUIRED)

# Executables
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_executable(cmerge cmerge.c)
target_link_libraries(cmerge cportage ${Popt_LIBRARY})

install(TARGETS
	cmerge
	RUNTIME DESTINATION bin)
