find_package(Qt6 COMPONENTS HttpServer REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)

set(LIB_NAME HttpServer)

aux_source_directory(. SRC_FILES)

add_library(${LIB_NAME} STATIC ${SRC_FILES})
add_library(MyServer::${LIB_NAME} ALIAS ${LIB_NAME})

set_target_properties(${LIB_NAME} PROPERTIES
        AUTOMOC ON
        AUTOUIC OFF
        AUTORCC ON
)

target_link_libraries(${LIB_NAME} PUBLIC
        Qt6::HttpServer
        Qt6::SerialPort
        Model::Data
        dataBase::Database
)


target_include_directories(${LIB_NAME} INTERFACE .)