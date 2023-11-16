cmake_minimum_required(VERSION 3.12)

project(deepin-deepinid-client
  VERSION "7.0.2"
  DESCRIPTION "deepin-deepinid-client"
  HOMEPAGE_URL "https://github.com/linuxdeepin/deepin-deepinid-client"
  LANGUAGES CXX C
)
 
add_subdirectory(src)

add_subdirectory(examples/unionid-app)
