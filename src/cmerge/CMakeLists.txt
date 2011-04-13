aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" sources)

add_executable(cmerge ${sources})
set_link_flags(cmerge)
target_link_libraries(cmerge cportage)

install(TARGETS cmerge RUNTIME DESTINATION bin)
