cmake_minimum_required(VERSION 3.16)

project(Projekt_Dateimanager_Dirk VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Multimedia)
# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS MultimediaWidgets)

include_directories(${PROJECT_SOURCE_DIR})

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Projekt_Dateimanager_Dirk
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        createdirectorydialog.h createdirectorydialog.cpp createdirectorydialog.ui
        focustrackingtreewidget.h focustrackingtreewidget.cpp
        dirutils.h dirutils.cpp
        settings.h settings.cpp
        viewers/textviewwindow.h viewers/textviewwindow.cpp viewers/textviewwindow.ui
        viewers/imageviewwindow.h viewers/imageviewwindow.cpp viewers/imageviewwindow.ui
        filetypedetection.h filetypedetection.cpp
        viewers/movieviewwindow.h viewers/movieviewwindow.cpp viewers/movieviewwindow.ui
        viewers/audioplayerwindow.h viewers/audioplayerwindow.cpp viewers/audioplayerwindow.ui
        fileinfowindow.h fileinfowindow.cpp fileinfowindow.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Projekt_Dateimanager_Dirk APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Projekt_Dateimanager_Dirk SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Projekt_Dateimanager_Dirk
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Projekt_Dateimanager_Dirk PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Projekt_Dateimanager_Dirk PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(Projekt_Dateimanager_Dirk PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(Projekt_Dateimanager_Dirk PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(Projekt_Dateimanager_Dirk PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
# target_link_libraries(Projekt_Dateimanager_Dirk PRIVATE Qt${QT_VERSION_MAJOR}::MultimediaWidgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Projekt_Dateimanager_Dirk)
endif()
set_target_properties(Projekt_Dateimanager_Dirk PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Projekt_Dateimanager_Dirk
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Projekt_Dateimanager_Dirk)
endif()
