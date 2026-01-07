cmake_minimum_required(VERSION 3.10)

project(libbthread
    VERSION 1.0.0
    DESCRIPTION "A simple threading library"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -O2)
add_compile_definitions(__arm__)

file(GLOB SOURCES "src/*.c")
add_library(bthread SHARED ${SOURCES})

target_include_directories(bthread PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(bthread PRIVATE m)

file(GLOB DEMO_SOURCES "demos/*.c")
foreach(DEMO_SOURCE ${DEMO_SOURCES})
    get_filename_component(DEMO_NAME ${DEMO_SOURCE} NAME_WE)
    add_executable(${DEMO_NAME} ${DEMO_SOURCE})
    target_link_libraries(${DEMO_NAME} PRIVATE bthread)
endforeach()

enable_testing()
file(GLOB TEST_SOURCES "tests/*_test.c")
foreach(TEST_SRC ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    target_include_directories(${TEST_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    target_link_libraries(${TEST_NAME} PRIVATE bthread)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
