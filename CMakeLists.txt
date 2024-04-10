cmake_minimum_required(VERSION 3.26)

project(Server LANGUAGES CXX VERSION 0.1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(DBG_BUILD_TESTS "Enables building tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(logMessageHandler)
add_subdirectory(data)
add_subdirectory(database)
add_subdirectory(deviceApp)
add_subdirectory(httpServer)
add_subdirectory(gui)
add_subdirectory(serverApp)

