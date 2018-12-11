set(CMAKE_CXX_STANDARD 14)

# http://doc.qt.io/qt-5/cmake-manual.html
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

add_definitions("-DQT_MESSAGELOGCONTEXT")

# Qt Config
find_package(PkgConfig REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5WebChannel CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)

# QCef config
pkg_search_module(QCef REQUIRED libqcef)
include_directories(${QCef_INCLUDE_DIRS})

# DtkConfig
pkg_search_module(DtkWidget REQUIRED dtkwidget)
include_directories(${DtkWidget_INCLUDE_DIRS})

set(CLIENT_FILES
    main.cpp
    login_window.h
    login_window.cpp
    sync_client.h
    sync_client.cpp
    deepinid_interface.h
    deepinid_interface.cpp
    web_event_delegate.h
    web_event_delegate.cpp)

add_executable(deepin-deepinid-client ${CLIENT_FILES})

install(TARGETS deepin-deepinid-client
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

target_link_libraries(deepin-deepinid-client Qt5::Widgets Qt5::WebChannel Qt5::DBus ${QCef_LDFLAGS} ${DtkWidget_LDFLAGS})

add_subdirectory(data)
