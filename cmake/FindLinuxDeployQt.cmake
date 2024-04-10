if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_program(LINUX_DEPLOY_APP
            NAMES linuxdeploy
            NO_CACHE
    )

    find_package(Qt6 COMPONENTS Core REQUIRED)
    get_target_property(QMAKE_LOCATION Qt6::qmake IMPORTED_LOCATION)

    message(DEBUG "Using qmake: ${QMAKE_LOCATION}")

    # target_linux_deploy_qt(TARGET deployedTargetName
    #                       [DEPLOY_DIR] deployDirectory - default CMAKE_BINARY_DIR
    #                       [NAME] deployTargetName      - default Package<deployedTargetName>
    #                       [PACKAGE_FILE_NAME] packageFileName - default <deployedTargetName>
    function(target_linux_deploy_qt)
        set(MULTI_ARG_LIST)
        set(SINGLE_ARG_LIST TARGET DEPLOY_DIR NAME PACKAGE_FILE_NAME)
        set(OPTIONS_LIST)

        cmake_parse_arguments(PREFIX "${OPTIONS_LIST}" "${SINGLE_ARG_LIST}" "${MULTI_ARG_LIST}" "${ARGN}")

        if (NOT PREFIX_TARGET)
            message(FATAL_ERROR "Provide TARGET argument")
        endif ()
        if (NOT PREFIX_DEPLOY_DIR)
            set(PREFIX_DEPLOY_DIR ${CMAKE_BINARY_DIR})
        endif ()

        if (NOT PREFIX_NAME)
            set(PREFIX_NAME Package${PREFIX_TARGET})
        endif ()

        if (TARGET ${PREFIX_NAME})
            message(FATAL_ERROR "Target ${PREFIX_NAME} already exists!")
        endif ()

        if (NOT PREFIX_PACKAGE_FILE_NAME)
            set(PREFIX_PACKAGE_FILE_NAME "${PREFIX_TARGET}")
        endif ()

        set(DEPLOY_PATH "${PREFIX_DEPLOY_DIR}/${PREFIX_TARGET}")
        set(ARCHIVE_PATH "${PREFIX_DEPLOY_DIR}/${PREFIX_PACKAGE_FILE_NAME}.tar.gz")

        add_custom_target(${PREFIX_NAME}
                COMMAND ${CMAKE_COMMAND} -E rm -rf ${DEPLOY_PATH}
                COMMAND export QMAKE=${QMAKE_LOCATION} && ${LINUX_DEPLOY_APP} --executable $<TARGET_FILE:${PREFIX_TARGET}> --plugin qt --appdir ${DEPLOY_PATH}
                COMMAND ${CMAKE_COMMAND} -E tar -czvf ${ARCHIVE_PATH} ${DEPLOY_PATH}
                WORKING_DIRECTORY ${PREFIX_DEPLOY_DIR}
                DEPENDS ${PREFIX_TARGET}
        )
    endfunction()

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(LinuxDeployQt
            REQUIRED_VARS LINUX_DEPLOY_APP
    )

else ()
    message(FATAL_ERROR "FindLinuxDeploy: Unsupported system.")
endif ()
