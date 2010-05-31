bison_target(ShellParser
    shellparser.y
    "${CMAKE_CURRENT_BINARY_DIR}/shellparser.c"
    COMPILE_FLAGS "-Wall -Werror")
flex_target(ShellScanner
    shellscanner.l
    "${CMAKE_CURRENT_BINARY_DIR}/shellscanner.c"
    COMPILE_FLAGS "--header-file=${CMAKE_CURRENT_BINARY_DIR}/shellscanner.h")
add_flex_bison_dependency(ShellScanner ShellParser)

add_definitions(-DG_LOG_DOMAIN="cportage" -DGETTEXT_PACKAGE="cportage")

# Executables
add_library(cportage SHARED
    atom.c io.c porttree.c settings.c strings.c
    ${BISON_ShellParser_OUTPUTS} ${FLEX_ShellScanner_OUTPUTS}
)
target_link_libraries(cportage ${GLIB2_LIBRARIES})

# Installation
install(TARGETS cportage LIBRARY DESTINATION lib)
