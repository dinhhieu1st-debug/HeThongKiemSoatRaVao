cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# 1. Database Unit Test
add_executable(
    test_cosodulieu
    unit/database/test_cosodulieu.cpp
    ../src/cosodulieu.cpp
    ../include/cosodulieu.h
)
target_link_libraries(
    test_cosodulieu
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Test
)

# 2. Config Unit Test
add_executable(
    test_cauhinhhethong
    unit/config/test_cauhinhhethong.cpp
    ../src/cauhinhhethong.cpp
    ../include/cauhinhhethong.h
)
target_link_libraries(
    test_cauhinhhethong
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Test
)

# 3. BLE Protocol Unit Test
add_executable(
    test_ble_protocol
    unit/ble_protocol/test_ble_protocol.cpp
    ../src/giaothucble.cpp
    ../include/giaothucble.h
)
target_link_libraries(
    test_ble_protocol
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Test
)

# 4. BLE Integration Test (Raspberry Pi <-> ESP32)
add_executable(
    test_ble_esp32
    integration/test_ble_esp32.cpp
    ../src/ketnoible.cpp
    ../include/ketnoible.h
    ../src/giaothucble.cpp
    ../include/giaothucble.h
)
target_link_libraries(
    test_ble_esp32
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Bluetooth
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Test
)
