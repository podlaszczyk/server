cmake_minimum_required(VERSION 3.26)

project(Server LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 EXACT 6.5.3 COMPONENTS Core REQUIRED)

add_subdirectory(data)
add_subdirectory(database)
add_subdirectory(device)
add_subdirectory(httpServer)

qt_standard_project_setup()
qt_add_executable(server
        main.cpp
)

target_link_libraries(server PRIVATE
        Model::Data
        dataBase::Database
        MyServer::HttpServer
)

qt_add_resources(server "assets"
        PREFIX
        "/"
        FILES
        assets/qt-logo.png
)