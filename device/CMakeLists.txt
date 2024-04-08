find_package(Qt6 EXACT 6.5.3 COMPONENTS Core REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)

qt_standard_project_setup()
qt_add_executable(Device
        main.cpp
        Device.cpp
)

target_link_libraries(Device PUBLIC
        Qt6::Core
        Qt6::SerialPort
)

