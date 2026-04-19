cmake_minimum_required(VERSION 3.16)

project(qt-commander VERSION 0.1 LANGUAGES CXX)

# create git-related constants
# -- get the current commit hash
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# -- get the commit date
execute_process(
  COMMAND git show -s --format=%ci
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_TIME
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

message("GIT_HASH is ${GIT_HASH}.")
message("GIT_TIME is ${GIT_TIME}.")

# replace git-related constants in GitInfos.cpp
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/util/GitInfos.template.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/util/GitInfos.cpp
               ESCAPE_QUOTES)
# end of git-related stuff

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
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS MultimediaWidgets)

include_directories(${PROJECT_SOURCE_DIR})

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        util/GitInfos.cpp
        util/GitInfos.hpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qt-commander
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        createdirectorydialog.h createdirectorydialog.cpp createdirectorydialog.ui
        focustrackingtreewidget.h focustrackingtreewidget.cpp
        util/dirutils.h util/dirutils.cpp
        util/settings.h util/settings.cpp
        viewers/textviewwindow.h viewers/textviewwindow.cpp viewers/textviewwindow.ui
        viewers/imageviewwindow.h viewers/imageviewwindow.cpp viewers/imageviewwindow.ui
        util/filetypedetection.h util/filetypedetection.cpp
        viewers/movieviewwindow.h viewers/movieviewwindow.cpp viewers/movieviewwindow.ui
        viewers/audioplayerwindow.h viewers/audioplayerwindow.cpp viewers/audioplayerwindow.ui
        viewers/videoplayerwindow.h viewers/videoplayerwindow.cpp viewers/videoplayerwindow.ui
        fileinfowindow.h fileinfowindow.cpp fileinfowindow.ui
        mountedvolumesdialog.h mountedvolumesdialog.cpp mountedvolumesdialog.ui
        util/durationutils.h util/durationutils.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET qt-commander APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(qt-commander SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(qt-commander
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(qt-commander PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(qt-commander PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(qt-commander PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(qt-commander PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(qt-commander PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(qt-commander PRIVATE Qt${QT_VERSION_MAJOR}::MultimediaWidgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.qt-commander)
endif()
set_target_properties(qt-commander PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS qt-commander
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qt-commander)
endif()
