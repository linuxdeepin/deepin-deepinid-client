set(CMAKE_CXX_STANDARD 14)

# http://doc.qt.io/qt-5/cmake-manual.html Find includes in corresponding build
# directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
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
find_package(Qt5LinguistTools REQUIRED)

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

add_executable(deepin-deepinid-client ${CLIENT_FILES} resource/resource.qrc)

# Executable files Generate .qm files from .ts files.
file(GLOB DMAN_TRANSLATION_TS ${CMAKE_SOURCE_DIR}/translations/*.ts)
set_source_files_properties(${DMAN_TRANSLATION_TS}
                            PROPERTIES
			    OUTPUT_LOCATION
			    ${CMAKE_BINARY_DIR}/translations)
qt5_add_translation(DMAN_TRANSLATION_QM ${DMAN_TRANSLATION_TS})
add_custom_target(update-qm DEPENDS ${DMAN_TRANSLATION_QM})

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${QCef_LIBDIR}/qcef")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set_target_properties(deepin-deepinid-client
                      PROPERTIES INSTALL_RPATH "${QCef_LIBDIR}/qcef")
target_link_libraries(deepin-deepinid-client ${QCef_LIBDIR}/qcef/libcef.so
                      ${LINK_LIBS})

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
		      ${QCef_LDFLAGS}
		      ${DtkWidget_LDFLAGS})

add_dependencies(deepin-deepinid-client update-qm)

add_subdirectory(data)
