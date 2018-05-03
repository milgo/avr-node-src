cmake_minimum_required(VERSION 3.2)

set(TARGET_NAME UploadPlugin)

project(${TARGET_NAME})

add_definitions(-DUPLOADPLUGIN_LIBRARY)

set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS
             Core)

add_definitions(${Qt5Widgets_DEFINITIONS})

add_library(${TARGET_NAME} SHARED UploadPlugin.cpp)

target_link_libraries(${TARGET_NAME} Qt5::Core)
