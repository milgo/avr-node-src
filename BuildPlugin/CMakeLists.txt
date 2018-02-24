cmake_minimum_required(VERSION 3.2)

project(BuildPlugin)
add_definitions(-DBUILDPLUGIN_LIBRARY)

set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS
             Core)

add_definitions(${Qt5Widgets_DEFINITIONS})

add_library(BuildPlugin SHARED BuildPlugin.cpp)

install(TARGETS BuildPlugin DESTINATION lib)
install(FILES BuildPlugin DESTINATION ./)

target_link_libraries(BuildPlugin Qt5::Core)

add_custom_command(TARGET BuildPlugin
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BuildPlugin> "${CMAKE_CURRENT_SOURCE_DIR}")
