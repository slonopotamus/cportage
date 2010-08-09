set(CMAKE_DOC_DIR "share/doc/cportage-${cportage_VERSION}"
    CACHE STRING
    "Path for documentation files. Will be prepended with CMAKE_INSTALL_PREFIX")

install(FILES "MagicEnvVars.txt"
    DESTINATION "${CMAKE_DOC_DIR}")
