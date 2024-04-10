find_package(Qt6 EXACT 6.5.3 COMPONENTS Core REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)

set(LIB_NAME Device)
qt_standard_project_setup()

aux_source_directory(. SRC_FILES)

add_library(${LIB_NAME} STATIC ${SRC_FILES})
add_library(Device::${LIB_NAME} ALIAS ${LIB_NAME})

set_target_properties(${LIB_NAME} PROPERTIES
        AUTOMOC ON
        AUTOUIC OFF
        AUTORCC ON
)

target_link_libraries(${LIB_NAME} PUBLIC
        Qt6::Core
        Qt6::SerialPort
        Logger::Logger
)

target_include_directories(${LIB_NAME} INTERFACE .)