set(CMAKE_MAN_DIR "/share/man"
    CACHE STRING
    "Path for documentation files. Will be prepended with CMAKE_INSTALL_PREFIX")

configure_file(cmerge.1.in "${CMAKE_CURRENT_BINARY_DIR}/cmerge.1")
configure_file(cportage.3.in "${CMAKE_CURRENT_BINARY_DIR}/cportage.3")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cmerge.1.in"
    DESTINATION "${CMAKE_MAN_DIR}/man1")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cportage.3.in"
    DESTINATION "${CMAKE_MAN_DIR}/man3")
