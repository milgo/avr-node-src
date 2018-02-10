cmake_minimum_required(VERSION 3.2)

project(ParsePlugin)
add_definitions(-DPARSEPLUGIN_LIBRARY)

set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS
             Core)

add_definitions(${Qt5Widgets_DEFINITIONS})

add_library(ParsePlugin SHARED ParsePlugin.cpp)

install(TARGETS ParsePlugin DESTINATION lib)
install(FILES ParsePlugin DESTINATION ./)

target_link_libraries(ParsePlugin Qt5::Core)

add_custom_command(TARGET ParsePlugin
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ParsePlugin> "${CMAKE_CURRENT_SOURCE_DIR}")
