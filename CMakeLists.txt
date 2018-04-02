project(avr-node-src)

add_subdirectory(ParsePlugin)
add_subdirectory(BuildPlugin)
add_subdirectory(DownloadPlugin)
add_subdirectory(MonitorPlugin)


set_target_properties( ParsePlugin BuildPlugin DownloadPlugin MonitorPlugin
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
