# Executables
add_library(cportage SHARED atom.c io.c object.c porttree.c settings.c strings.c)
target_link_libraries(cportage ${PCRE_LIBRARY} "iberty")

# Installation
install(TARGETS
  cportage
  LIBRARY DESTINATION lib)
