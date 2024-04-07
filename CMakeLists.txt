cmake_minimum_required(VERSION 3.26)

project(Server LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 COMPONENTS HttpServer REQUIRED)
find_package(Qt6 EXACT 6.5.3 COMPONENTS Widgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)

project(server)

add_subdirectory(device)

qt_standard_project_setup()
qt_add_executable(server
        main.cpp
        Sender.cpp
)

target_link_libraries(server PUBLIC
        Qt6::Widgets
        Qt6::HttpServer
        Qt6::SerialPort
)

qt_add_resources(server "assets"
        PREFIX
        "/"
        FILES
        assets/qt-logo.png
)