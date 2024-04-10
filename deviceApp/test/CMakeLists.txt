find_package(Catch2 REQUIRED)
find_package(fakeit REQUIRED)

set(TEST_NAME DeviceTests)

add_executable(${TEST_NAME}
        DeviceTest.cpp)


target_link_libraries(${TEST_NAME} PRIVATE
        Device::Device
        Catch2::Catch2WithMain
        fakeit::fakeit
)

add_test(NAME ${TEST_NAME}
        COMMAND ${TEST_NAME} -d yes --order lex -v high --reporter JUnit::out=${TEST_REPORT_DIRECTORY}/${TEST_NAME}.xml --reporter console::out=-::colour-mode=ansi
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_tests_properties(${TEST_NAME} PROPERTIES LABELS "Unit Test")
