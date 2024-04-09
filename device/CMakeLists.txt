find_package(Qt6 EXACT 6.5.3 COMPONENTS Core REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)

set(APP_NAME deviceApp)
qt_standard_project_setup()
qt_add_executable(${APP_NAME}
        main.cpp
        Device.cpp
)

target_link_libraries(${APP_NAME} PUBLIC
        Qt6::Core
        Qt6::SerialPort
        Logger::Logger
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(LinuxDeployQt REQUIRED)

    target_linux_deploy_qt(TARGET ${APP_NAME}
            PACKAGE_FILE_NAME "${APP_NAME}-Linux-x86_64"
    )
endif ()