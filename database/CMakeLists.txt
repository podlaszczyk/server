find_package(Qt6 REQUIRED COMPONENTS Sql)

set(LIB_NAME Database)

aux_source_directory(. SRC_FILES)

add_library(${LIB_NAME} STATIC ${SRC_FILES})
add_library(dataBase::${LIB_NAME} ALIAS ${LIB_NAME})

set_target_properties(${LIB_NAME} PROPERTIES
        AUTOMOC ON
        AUTOUIC OFF
        AUTORCC ON
)

target_link_libraries(${LIB_NAME} PUBLIC
        Qt6::Sql
        Model::Data
)

target_include_directories(${LIB_NAME} INTERFACE .)