cmake_minimum_required(VERSION 3.2)

project(MonitorPlugin)
add_definitions(-DMONITORPLUGIN_LIBRARY)

set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS
             Core)

add_definitions(${Qt5Widgets_DEFINITIONS})

add_library(MonitorPlugin SHARED MonitorPlugin.cpp)

install(TARGETS MonitorPlugin DESTINATION lib)
install(FILES MonitorPlugin DESTINATION ./)

target_link_libraries(MonitorPlugin Qt5::Core)

qt5_use_modules(MonitorPlugin SerialPort)

add_custom_command(TARGET MonitorPlugin
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:MonitorPlugin> "${CMAKE_CURRENT_SOURCE_DIR}")
