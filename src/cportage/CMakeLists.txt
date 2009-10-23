# Dependencies
find_package(PCRE REQUIRED)

# Executables
add_library(cportage SHARED atom.c object.c porttree.c settings.c strings.c)
target_link_libraries(cportage ${PCRE_LIBRARY})

# Installation
install(TARGETS
	cportage
	LIBRARY DESTINATION lib)
