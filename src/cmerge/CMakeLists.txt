# Dependencies
find_package(Popt REQUIRED)

# Executables
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_executable(cmerge clean_action.c cmerge.c info_action.c install_action.c
	search_action.c)
target_link_libraries(cmerge cportage ${Popt_LIBRARY})

install(TARGETS
	cmerge
	RUNTIME DESTINATION bin)
