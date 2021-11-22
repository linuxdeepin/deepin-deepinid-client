set(CMAKE_CXX_STANDARD 14)

# http://doc.qt.io/qt-5/cmake-manual.html Find includes in corresponding build
# directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_FLAGS "-Wl,-as-need -fPIE")
set(CMAKE_EXE_LINKER_FLAGS "-pie")

add_definitions("-DQT_MESSAGELOGCONTEXT")
add_compile_definitions(CVERSION="${CVERSION}")

# Qt Config
find_package(PkgConfig REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5WebChannel CONFIG REQUIRED)
find_package(Qt5WebEngine CONFIG REQUIRED)
find_package(Qt5WebEngineCore CONFIG REQUIRED)
find_package(Qt5WebEngineWidgets CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# DtkConfig
pkg_search_module(DtkWidget REQUIRED dtkwidget)
include_directories(${DtkWidget_INCLUDE_DIRS})

# dbus_Frame
pkg_search_module(DFrameworkDBus REQUIRED dframeworkdbus)
include_directories(${DFrameworkDBus_INCLUDE_DIRS})

set(CLIENT_FILES
        ui/login_window.h
        ui/login_window.cpp
        ui/sync_client.h
        ui/sync_client.cpp
        ui/login_page.h
        ui/login_page.cpp
        ui/login_view.h
        ui/login_view.cpp
        ui/update_client.h
        ui/update_client.cpp
        utils/utils.cpp
        utils/utils.h
        ipc/const.h
        ipc/deepinid_interface.h
        ipc/deepinid_interface.cpp
        ipc/dbuslogin1manager.h
        ipc/dbuslogin1manager.cpp
        ipc/dbusvariant.h
        ipc/dbusvariant.cpp
        service/session.h
        service/session.cpp
        service/authentication_manager.cpp
        service/authentication_manager.h
        service/authorize.h
        main.cpp)

add_executable(deepin-deepinid-client ${CLIENT_FILES} resource/resource.qrc)

# Executable files Generate .qm files from .ts files.
file(GLOB DMAN_TRANSLATION_TS ${CMAKE_SOURCE_DIR}/translations/*.ts)
set_source_files_properties(${DMAN_TRANSLATION_TS}
        PROPERTIES
        OUTPUT_LOCATION
        ${CMAKE_BINARY_DIR}/translations)
qt5_add_translation(DMAN_TRANSLATION_QM ${DMAN_TRANSLATION_TS})
add_custom_target(update-qm DEPENDS ${DMAN_TRANSLATION_QM})

install(TARGETS deepin-deepinid-client DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

install(DIRECTORY ${CMAKE_BINARY_DIR}/translations
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/deepin-deepinid-client
        FILES_MATCHING
        PATTERN "*")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/com.deepin.deepinid.Client.service
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/dbus-1/services/)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/web/com.deepin.deepinid.Client.svg
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps/)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resource/privacy
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/deepin-deepinid-client
        FILES_MATCHING
        PATTERN "*")

target_link_libraries(deepin-deepinid-client
              Qt5::Widgets
              Qt5::WebChannel
              Qt5::DBus
              Qt5::WebEngineWidgets
              ${DtkWidget_LDFLAGS}
              ${DFrameworkDBus_LIBRARIES})

add_dependencies(deepin-deepinid-client update-qm)

add_subdirectory(data)
