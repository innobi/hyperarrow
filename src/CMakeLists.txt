file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${HyperArrow_SOURCE_DIR}/include/hyperarrow/*.h")

add_library(hyperarrow_writer SHARED writer.cc types.cc ${HEADER_LIST})
target_include_directories(hyperarrow_writer PUBLIC ../include)
target_link_libraries(hyperarrow_writer PRIVATE Tableau::tableauhyperapi-cxx)
target_link_libraries(hyperarrow_writer PRIVATE arrow_shared)

add_library(hyperarrow_reader SHARED reader.cc types.cc ${HEADER_LIST})
target_include_directories(hyperarrow_reader PUBLIC ../include)
target_link_libraries(hyperarrow_reader PRIVATE Tableau::tableauhyperapi-cxx)
target_link_libraries(hyperarrow_reader PRIVATE arrow_shared)
