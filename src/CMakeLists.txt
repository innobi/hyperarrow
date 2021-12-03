file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${HyperArrow_SOURCE_DIR}/include/hyperarrow/*.h")

add_library(hyperarrow_writer SHARED writer.cc types.cc ${HEADER_LIST})
target_include_directories(hyperarrow_writer PUBLIC ../include)
target_link_libraries(hyperarrow_writer PRIVATE Tableau::tableauhyperapi-cxx)
target_link_libraries(hyperarrow_writer PRIVATE arrow_shared)

add_library(hyperarrow_reader SHARED reader.cc types.cc ${HEADER_LIST})
target_include_directories(hyperarrow_reader PUBLIC ../include)
target_link_libraries(hyperarrow_reader PRIVATE Tableau::tableauhyperapi-cxx)
target_link_libraries(hyperarrow_reader PRIVATE arrow_shared)

# Copy to Python folder after build
add_custom_command(
    TARGET hyperarrow_writer
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
	${CMAKE_SOURCE_DIR}/python/src/hyperarrow/lib/
)

add_custom_command(
    TARGET hyperarrow_writer
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:hyperarrow_writer>
	${CMAKE_SOURCE_DIR}/python/src/hyperarrow/lib/
)

add_custom_command(
    TARGET hyperarrow_writer
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:hyperarrow_reader> 
	${CMAKE_SOURCE_DIR}/python/src/hyperarrow/lib/
)

