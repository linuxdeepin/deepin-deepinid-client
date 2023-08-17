cmake_minimum_required(VERSION 3.12)

set(CVERSION "6.0.1" CACHE STRING "version defined")

project(deepin-deepinid-client
  VERSION ${CVERSION}
  DESCRIPTION "deepin-deepinid-client"
  HOMEPAGE_URL "https://github.com/linuxdeepin/deepin-deepinid-client"
  LANGUAGES CXX C
)
 
add_subdirectory(src)

add_subdirectory(examples/unionid-app)
