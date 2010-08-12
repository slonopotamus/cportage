configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

# Executables

add_definitions(-DG_LOG_DOMAIN="cmerge")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(cmerge
    clean_action.c cmerge.c info_action.c install_action.c search_action.c)
target_link_libraries(cmerge cportage)

install(TARGETS cmerge RUNTIME DESTINATION bin)
