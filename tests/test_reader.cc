#define BOOST_TEST_MODULE hyperarrow_reader_tests
#include <arrow/api.h>
#include <boost/test/included/unit_test.hpp>
#include <hyperarrow/reader.h>

BOOST_AUTO_TEST_CASE(test_basic_read) {
  auto result =
      hyperarrow::arrowTableFromHyper("data/example.hyper", "schema", "table");
  if (result.ok()) {
    auto table = result.ValueOrDie();
    auto array16 =
        std::static_pointer_cast<arrow::Int16Array>(table->column(0)->chunk(0));
    BOOST_TEST(array16->Value(0) == 16);
    BOOST_TEST(!array16->IsValid(1));
    // TODO: test value(1) == NULL for each of these

    auto array32 =
        std::static_pointer_cast<arrow::Int32Array>(table->column(1)->chunk(0));
    BOOST_TEST(array32->Value(0) == 32);
    BOOST_TEST(!array32->IsValid(1));

    auto array64 =
        std::static_pointer_cast<arrow::Int64Array>(table->column(2)->chunk(0));
    BOOST_TEST(array64->Value(0) == 64);
    BOOST_TEST(!array64->IsValid(1));

    // Floats cannot be round-tripped via hyper; come back as double
    // so expect some precision loss in round tripping
    auto arrayF = std::static_pointer_cast<arrow::DoubleArray>(
        table->column(3)->chunk(0));
    BOOST_TEST(abs(arrayF->Value(0) - 1.234) <= 1.0e-007);

    auto arrayD = std::static_pointer_cast<arrow::DoubleArray>(
        table->column(4)->chunk(0));
    BOOST_TEST(arrayD->Value(0) == 123.4);
    BOOST_TEST(!arrayD->IsValid(1));

    auto arrayBool = std::static_pointer_cast<arrow::BooleanArray>(
        table->column(5)->chunk(0));
    BOOST_TEST(arrayBool->Value(0) == true);
    BOOST_TEST(!arrayBool->IsValid(1));

    auto arrayDate = std::static_pointer_cast<arrow::Date32Array>(
        table->column(6)->chunk(0));
    BOOST_TEST(arrayDate->Value(0) == 0);
    BOOST_TEST(!arrayDate->IsValid(1));

    auto arrayString = std::static_pointer_cast<arrow::StringArray>(
        table->column(7)->chunk(0));
    BOOST_TEST(arrayString->Value(0) == "a");
    BOOST_TEST(!arrayString->IsValid(1));

    auto arrayTs = std::static_pointer_cast<arrow::TimestampArray>(
        table->column(8)->chunk(0));
    BOOST_TEST(arrayTs->Value(0) == 0);
    BOOST_TEST(!arrayTs->IsValid(1));
  }
}
