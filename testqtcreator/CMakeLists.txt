cmake_minimum_required(VERSION 3.16)

project(TestHeThongQt LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    Qt6
    REQUIRED COMPONENTS
    Core
    Sql
    Test
)

enable_testing()

qt_add_executable(
    test_he_thong_qt
    test_he_thong_qt.cpp
    ../src/cauhinhhethong.cpp
    ../src/cosodulieu.cpp
    ../include/cauhinhhethong.h
    ../include/cosodulieu.h
)

target_include_directories(
    test_he_thong_qt
    PRIVATE
    ../include
)

target_link_libraries(
    test_he_thong_qt
    PRIVATE
    Qt6::Core
    Qt6::Sql
    Qt6::Test
)

add_test(
    NAME he_thong_qt
    COMMAND test_he_thong_qt
)

set_tests_properties(
    he_thong_qt
    PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
