cmake_minimum_required(VERSION 3.2)

set(TARGET_NAME BuildPlugin)

project(${TARGET_NAME})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCHITECTURE_BITS "x64")
 else()
  set(ARCHITECTURE_BITS "i386")
endif()

add_definitions(-DBUILDPLUGIN_LIBRARY)

set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS
             Core)

add_definitions(${Qt5Widgets_DEFINITIONS})

add_library(${TARGET_NAME} SHARED BuildPlugin.cpp)

install(TARGETS ${TARGET_NAME} DESTINATION lib)
install(FILES ${TARGET_NAME} DESTINATION ./)

target_link_libraries(${TARGET_NAME} Qt5::Core)

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "${TARGET_NAME}-${CMAKE_SYSTEM_NAME}-${ARCHITECTURE_BITS}")

add_custom_command(TARGET BuildPlugin
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BuildPlugin> "${CMAKE_CURRENT_SOURCE_DIR}")
