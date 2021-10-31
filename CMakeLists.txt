cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)

project(HyperArrow)

option(ARROW_LINK_SHARED "Link to the Arrow shared library" ON)

find_package(Arrow REQUIRED)

if (NOT DEFINED CMAKE_PREFIX_PATH)
  set(CMAKE_PREFIX_PATH "/build/tableauhyperapi/share/cmake" CACHE PATH "Cmake prefix path" FORCE)
endif ()
find_package(tableauhyperapi-cxx REQUIRED CONFIG)

set(HYPERARROW_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/hyperarrow)
set(SOURCES
  ${HYPERARROW_SRC_DIR}/types.cc
  ${HYPERARROW_SRC_DIR}/writer.cc
  ${HYPERARROW_SRC_DIR}/hyperarrow.cc
  )
  
add_executable(hyperarrow ${SOURCES})
target_include_directories(hyperarrow PRIVATE ${HYPERARROW_SRC_DIR})
target_link_libraries(hyperarrow PRIVATE Tableau::tableauhyperapi-cxx)
target_link_libraries(hyperarrow PRIVATE arrow_shared)
