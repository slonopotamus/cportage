#bison_target(ShellParser shellparser.y ${CMAKE_CURRENT_BINARY_DIR}/shellparser.c)
#flex_target(ShellScanner shelllexer.l  ${CMAKE_CURRENT_BINARY_DIR}/shelllexer.c)
#add_flex_bison_dependency(ShellScanner ShellParser)

add_definitions(-DG_LOG_DOMAIN="cportage" -DGETTEXT_PACKAGE="cportage")

# Executables
add_library(cportage SHARED
    atom.c io.c porttree.c settings.c ${BISON_ShellParser_OUTPUTS} ${FLEX_ShellScanner_OUTPUTS})
target_link_libraries(cportage ${GLIB2_LIBRARIES})

# Installation
install(TARGETS
  cportage
  LIBRARY DESTINATION lib)
