find_package(Qt6 EXACT 6.5.3 COMPONENTS Widgets REQUIRED)
find_package(Qt6 EXACT 6.5.3 COMPONENTS Network REQUIRED)

qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

qt_add_executable(gui
        example.qrc
        main.cpp
        MainWindow.cpp
        CloneWidget.cpp
)

target_link_libraries(gui PRIVATE
        Qt6::Widgets
        Qt6::Network
)

qt_add_resources(gui "images"
        PREFIX "/"
        FILES
        images/clone_signet_big_white.png
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(LinuxDeployQt REQUIRED)

    target_linux_deploy_qt(TARGET gui
            PACKAGE_FILE_NAME "gui-Linux-x86_64"
    )
endif ()