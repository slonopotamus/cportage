# Dependencies
find_package(PCRE REQUIRED)

# Executables
add_library(cportage SHARED atom.c settings.c object.c)
target_link_libraries(cportage ${PCRE_LIBRARY})

# Installation
install(TARGETS
	cportage
	LIBRARY DESTINATION lib)
