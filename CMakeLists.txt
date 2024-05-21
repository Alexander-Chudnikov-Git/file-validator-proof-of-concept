cmake_minimum_required(VERSION 3.21.1)

include(cmake/build_protection.cmake)

set(PROJECT_NAME "file_validator" LANGUAGES CXX)
project(${PROJECT_NAME})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS
    Core
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
)

set(QT_LINKING_LIBS
    Qt${QT_VERSION_MAJOR}::Core
)

set(VALIDATOR_INCLUDE_DIRS
    "src/"
    "src/file/"
    "src/file/writer/"
    "src/file/reader/"
    "src/file/validator/"
    ${Qt${QT_VERSION_MAJOR}Core_INCLUDE_DIRS}
)

file(GLOB PROJECT_BASE CONFIGURE_DEPENDS
    "src/*.hpp"
    "src/*.cpp"
)

file(GLOB FILE_PROCESSING_SRC CONFIGURE_DEPENDS
    "src/file/*.hpp"
    "src/file/writer/*.hpp"
    "src/file/writer/*.cpp"
    "src/file/reader/*.hpp"
    "src/file/reader/*.cpp"
    "src/file/validator/*.hpp"
    "src/file/validator/*.cpp"
)

file(GLOB RESOURCES_QRC CONFIGURE_DEPENDS
    "resources/*.qrc"
)

source_group("Project Base" FILES ${PROJECT_BASE})
source_group("File Related Stuff" FILES ${FILE_PROCESSING_SRC})
source_group("Resources" FILES ${RESOURCES_QRC})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_BASE}
    )
else()
    add_executable(${PROJECT_NAME} ${PROJECT_BASE})
endif()

if(FILE_PROCESSING_SRC)
    add_library(libfileprocessing STATIC ${FILE_PROCESSING_SRC})
    target_include_directories(libfileprocessing PRIVATE ${VALIDATOR_INCLUDE_DIRS})
    target_link_directories(libfileprocessing PRIVATE ${VALIDATOR_INCLUDE_DIRS})
    target_link_libraries(${QT_LINKING_LIBS})
endif()

if(RESOURCES_QRC)
    qt5_add_resources(RESOURCES_ADDED ${RESOURCES_QRC})
    target_sources(${PROJECT_NAME} PUBLIC ${RESOURCES_ADDED})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${VALIDATOR_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${VALIDATOR_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE
    libfileprocessing
    ${QT_LINKING_LIBS}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
