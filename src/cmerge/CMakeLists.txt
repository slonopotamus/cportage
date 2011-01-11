file(GLOB sources *.c)

add_executable(cmerge ${sources})
set_link_flags(cmerge)
target_link_libraries(cmerge cportage)

install(TARGETS cmerge RUNTIME DESTINATION bin)
