add_executable(hyperarrow hyperarrow.cc)
target_link_libraries(hyperarrow PRIVATE hyperarrow_writer)
target_link_libraries(hyperarrow PRIVATE Tableau::tableauhyperapi-cxx)
target_link_libraries(hyperarrow PRIVATE arrow_shared)
