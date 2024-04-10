add_subdirectory(device)

set(APP_NAME DeviceApp)

qt_add_executable(${APP_NAME}
        main.cpp
)

target_link_libraries(${APP_NAME} PRIVATE
        Device::Device
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(LinuxDeployQt REQUIRED)

    target_linux_deploy_qt(TARGET ${APP_NAME}
            PACKAGE_FILE_NAME "${APP_NAME}-Linux-x86_64"
    )
endif ()

#if ($CACHE{DBG_BUILD_TESTS})
#    add_subdirectory(test)
#endif ()
