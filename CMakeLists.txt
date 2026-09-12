cmake_minimum_required(VERSION 3.16)

project(Hethongkiemsoatravao VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    QT
    NAMES Qt6 Qt5
    REQUIRED COMPONENTS
    Widgets
    Sql
    DBus
    Bluetooth
)

find_package(
    Qt${QT_VERSION_MAJOR}
    REQUIRED COMPONENTS
    Widgets
    Sql
    DBus
    Bluetooth
)

set(NGUON_CHUONG_TRINH
    src/main.cpp

    src/bandichtiengvietqt.cpp
    include/bandichtiengvietqt.h

    src/cuasochinh.cpp
    include/cuasochinh.h
    ui/cuasochinh.ui

    src/dangnhap.cpp
    include/dangnhap.h

    src/taotaikhoan.cpp
    include/taotaikhoan.h

    src/cosodulieu.cpp
    include/cosodulieu.h

    src/ketnoible.cpp
    include/ketnoible.h

    src/cauhinhhethong.cpp
    include/cauhinhhethong.h

    src/caidathethong.cpp
    include/caidathethong.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(
        Hethongkiemsoatravao
        MANUAL_FINALIZATION
        ${NGUON_CHUONG_TRINH}
    )
else()
    add_executable(
        Hethongkiemsoatravao
        ${NGUON_CHUONG_TRINH}
    )
endif()

target_include_directories(
    Hethongkiemsoatravao
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    Hethongkiemsoatravao
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Bluetooth
)

set_target_properties(
    Hethongkiemsoatravao
    PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(
        Hethongkiemsoatravao
    )
endif()
