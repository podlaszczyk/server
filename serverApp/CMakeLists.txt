find_package(Qt6 EXACT 6.5.3 COMPONENTS Core REQUIRED)

set(APPLICATION ServerApp)

qt_standard_project_setup()

qt_add_executable(${APPLICATION}
        main.cpp
)

target_link_libraries(${APPLICATION} PRIVATE
        Model::Data
        dataBase::Database
        MyServer::HttpServer
        Logger::Logger
)
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(LinuxDeployQt REQUIRED)

    target_linux_deploy_qt(TARGET ${APPLICATION}
            PACKAGE_FILE_NAME "${APPLICATION}-Linux-x86_64"
    )
endif ()