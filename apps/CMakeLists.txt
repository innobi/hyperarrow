add_executable(hyperarrow hyperarrow.cc)
target_link_libraries(hyperarrow PRIVATE hyperarrow_writer)
target_link_libraries(hyperarrow PRIVATE Tableau::tableauhyperapi-cxx)
target_link_libraries(hyperarrow PRIVATE arrow_shared)

add_executable(test_reader1 test_reader.cc)
target_link_libraries(test_reader1 PRIVATE hyperarrow_reader)
target_link_libraries(test_reader1 PRIVATE Tableau::tableauhyperapi-cxx)
target_link_libraries(test_reader1 PRIVATE arrow_shared)
