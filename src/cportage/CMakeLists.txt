bison_target(ShellParser shellparser.y ${CMAKE_CURRENT_BINARY_DIR}/shellparser.c)
flex_target(ShellScanner shelllexer.l  ${CMAKE_CURRENT_BINARY_DIR}/shelllexer.c)
add_flex_bison_dependency(ShellScanner ShellParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Executables
add_library(cportage SHARED atom.c io.c object.c porttree.c settings.c strings.c ${BISON_ShellParser_OUTPUTS} ${FLEX_ShellScanner_OUTPUTS})
target_link_libraries(cportage ${PCRE_LIBRARY} iberty)

# Installation
install(TARGETS
  cportage
  LIBRARY DESTINATION lib)
