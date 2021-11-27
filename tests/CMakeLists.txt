add_executable(test_writer test_writer.cc)
target_link_libraries(test_writer PRIVATE hyperarrow_writer)
target_link_libraries(test_writer PRIVATE Tableau::tableauhyperapi-cxx)
target_link_libraries(test_writer PRIVATE arrow_shared)
target_link_libraries(test_writer PRIVATE ${Boost_LIBRARIES})

add_test(test_writer test_writer)
