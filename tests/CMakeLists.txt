add_executable(test_roundtrip test_roundtrip.cc)
target_link_libraries(test_roundtrip PRIVATE hyperarrow_writer)
target_link_libraries(test_roundtrip PRIVATE hyperarrow_reader)
target_link_libraries(test_roundtrip PRIVATE Tableau::tableauhyperapi-cxx)
target_link_libraries(test_roundtrip PRIVATE arrow_shared)
target_link_libraries(test_roundtrip PRIVATE ${Boost_LIBRARIES})

add_test(test_roundtrip test_roundtrip)

add_executable(test_reader test_reader.cc)
target_link_libraries(test_reader PRIVATE hyperarrow_writer)
target_link_libraries(test_reader PRIVATE hyperarrow_reader)
target_link_libraries(test_reader PRIVATE Tableau::tableauhyperapi-cxx)
target_link_libraries(test_reader PRIVATE arrow_shared)
target_link_libraries(test_reader PRIVATE ${Boost_LIBRARIES})

add_test(test_reader test_reader)
