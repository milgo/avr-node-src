cmake_minimum_required(VERSION 3.2)

project(DownloadPlugin)
add_definitions(-DDOWNLOADPLUGIN_LIBRARY)

set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS
             Core)

add_definitions(${Qt5Widgets_DEFINITIONS})

add_library(DownloadPlugin SHARED DownloadPlugin.cpp)

install(TARGETS DownloadPlugin DESTINATION lib)
install(FILES DownloadPlugin DESTINATION ./)

target_link_libraries(DownloadPlugin Qt5::Core)

add_custom_command(TARGET DownloadPlugin
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:DownloadPlugin> "${CMAKE_CURRENT_SOURCE_DIR}")
