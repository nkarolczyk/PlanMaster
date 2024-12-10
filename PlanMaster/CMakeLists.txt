cmake_minimum_required(VERSION 3.5)

project(PlanMaster VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

# Wskazanie ścieżek do katalogów build, gdzie znajdują się pliki generowane przez uic
set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_BINARY_DIR}/PlanMaster_autogen/include
)

# Źródła projektu
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    task.h
    task.cpp
    taskmanager.cpp
    taskmanager.h
    logindialog.cpp
    logindialog.h
    logindialog.ui
    registerdialog.cpp
    registerdialog.h
    registerdialog.ui
    databasemanager.h
    databasemanager.cpp
    resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PlanMaster
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(PlanMaster SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(PlanMaster
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Dodanie katalogu build/PlanMaster_autogen/include do ścieżek dołączania
target_include_directories(PlanMaster PRIVATE
    ${CMAKE_BINARY_DIR}/PlanMaster_autogen/include
)

# Linkowanie bibliotek Qt
target_link_libraries(PlanMaster PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

# Ustawienia dla macOS/iOS
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.PlanMaster)
endif()
set_target_properties(PlanMaster PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Instalacja
include(GNUInstallDirs)
install(TARGETS PlanMaster
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalizacja dla Qt 6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PlanMaster)
endif()
