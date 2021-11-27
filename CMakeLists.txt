# Much inspiration here borrow from
# https://gitlab.com/CLIUtils/modern-cmake/tree/master/examples/extended-project
# https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html
cmake_minimum_required(VERSION 3.0)
set (CMAKE_CXX_STANDARD 11)

project(HyperArrow VERSION 0.1)

option(ARROW_LINK_SHARED "Link to the Arrow shared library" ON)

find_package(Arrow REQUIRED)

if (NOT DEFINED CMAKE_PREFIX_PATH)
  set(CMAKE_PREFIX_PATH $ENV{TABLEAU_CMAKE_PATH} CACHE PATH "Cmake prefix path" FORCE)
endif ()
  
find_package(tableauhyperapi-cxx REQUIRED CONFIG)
find_package(Boost COMPONENTS system filesystem REQUIRED)

add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(tests)
