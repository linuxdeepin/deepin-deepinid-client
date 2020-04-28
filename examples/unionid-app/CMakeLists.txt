cmake_minimum_required(VERSION 3.5)

project(unionid-app LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)

add_executable(unionid-app main.cpp app.cpp app.h)
target_link_libraries(unionid-app Qt5::Core Qt5::DBus)
