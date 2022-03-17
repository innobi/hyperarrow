#define BOOST_TEST_MODULE hyperarrow_reader_tests

#include <hyperarrow/reader.h>
#include <hyperarrow/writer.h>

#include <arrow/api.h>
#include <boost/test/included/unit_test.hpp>

#define ABORT_ON_FAILURE(expr)                                                 \
  do {                                                                         \
    arrow::Status status_ = (expr);                                            \
    if (!status_.ok()) {                                                       \
      std::cerr << status_.message() << std::endl;                             \
      abort();                                                                 \
    }                                                                          \
  } while (0);

BOOST_AUTO_TEST_CASE(test_string_truncation_issue) {
  auto schema = arrow::schema(
      {arrow::field("foo", arrow::utf8()), arrow::field("bar", arrow::utf8())});

  arrow::StringBuilder foobuilder;
  arrow::StringBuilder barbuilder;
  std::shared_ptr<arrow::Array> array_foo;
  std::shared_ptr<arrow::Array> array_bar;

  ABORT_ON_FAILURE(foobuilder.Append("a"));
  ABORT_ON_FAILURE(foobuilder.Finish(&array_foo));

  ABORT_ON_FAILURE(barbuilder.Append(std::string(1000, 'b')));
  ABORT_ON_FAILURE(barbuilder.Finish(&array_bar));

  auto table = arrow::Table::Make(schema, {array_foo, array_bar});

  const char path[] = "example.hyper";
  hyperarrow::arrowTableToHyper(table, path, "schema", "table");

  auto result = hyperarrow::arrowTableFromHyper(path, "schema", "table");
  if (result.ok()) {
    auto read = result.ValueOrDie();
    BOOST_TEST(table->Equals(*read));
  } else {
    BOOST_ERROR("Could not read file");
  }

  remove(path);
  remove("hyperd.log");
}
