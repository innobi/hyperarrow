#define BOOST_TEST_MODULE hyperarrow_writer_tests
#include <stdio.h>
#include <arrow/api.h>
#include <boost/test/included/unit_test.hpp>
#include <hyperarrow/reader.h>
#include <hyperarrow/writer.h>

#define ABORT_ON_FAILURE(expr)                                                 \
  do {                                                                         \
    arrow::Status status_ = (expr);                                            \
    if (!status_.ok()) {                                                       \
      std::cerr << status_.message() << std::endl;                             \
      abort();                                                                 \
    }                                                                          \
  } while (0);

BOOST_AUTO_TEST_CASE(test_basic_write) {
  auto schema = arrow::schema(
      {arrow::field("a", arrow::int16()), arrow::field("b", arrow::int32()),
       arrow::field("c", arrow::int64()), arrow::field("d", arrow::float32()),
       arrow::field("e", arrow::float64()), arrow::field("f", arrow::boolean()),
       arrow::field("g", arrow::date32()), arrow::field("h", arrow::utf8()),
       arrow::field("i", arrow::timestamp(arrow::TimeUnit::MICRO))});

  arrow::MemoryPool *pool = arrow::default_memory_pool();
  arrow::Int16Builder int16builder(pool);
  arrow::Int32Builder int32builder(pool);
  arrow::Int64Builder int64builder(pool);
  arrow::FloatBuilder floatbuilder(pool);
  arrow::DoubleBuilder doublebuilder(pool);
  arrow::BooleanBuilder boolbuilder(pool);
  arrow::StringBuilder stringbuilder(pool);
  arrow::Date32Builder date32builder(pool);
  arrow::TimestampBuilder tsbuilder(arrow::timestamp(arrow::TimeUnit::MICRO),
                                    pool);
  std::shared_ptr<arrow::Array> array_a;
  std::shared_ptr<arrow::Array> array_b;
  std::shared_ptr<arrow::Array> array_c;
  std::shared_ptr<arrow::Array> array_d;
  std::shared_ptr<arrow::Array> array_e;
  std::shared_ptr<arrow::Array> array_f;
  std::shared_ptr<arrow::Array> array_g;
  std::shared_ptr<arrow::Array> array_h;
  std::shared_ptr<arrow::Array> array_i;

  ABORT_ON_FAILURE(int16builder.AppendValues({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
  ABORT_ON_FAILURE(int16builder.AppendNull());
  ABORT_ON_FAILURE(int16builder.Finish(&array_a));

  ABORT_ON_FAILURE(int32builder.AppendValues({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
  ABORT_ON_FAILURE(int32builder.AppendNull());
  ABORT_ON_FAILURE(int32builder.Finish(&array_b));

  ABORT_ON_FAILURE(int64builder.AppendValues({1, 2, 1, 2, 1, 2, 1, 2, 1, 2}));
  ABORT_ON_FAILURE(int64builder.AppendNull());
  ABORT_ON_FAILURE(int64builder.Finish(&array_c));

  ABORT_ON_FAILURE(
      floatbuilder.AppendValues({0., 1., 2., 3., 4., 5., 6., 7., 8., 9.}));
  ABORT_ON_FAILURE(floatbuilder.AppendNull());
  ABORT_ON_FAILURE(floatbuilder.Finish(&array_d));

  ABORT_ON_FAILURE(
      doublebuilder.AppendValues({0., 1., 2., 3., 4., 5., 6., 7., 8., 9.}));
  ABORT_ON_FAILURE(doublebuilder.AppendNull());
  ABORT_ON_FAILURE(doublebuilder.Finish(&array_e));

  ABORT_ON_FAILURE(boolbuilder.AppendValues(std::vector<bool>{
      true, false, true, false, true, false, true, false, true, false}));
  ABORT_ON_FAILURE(boolbuilder.AppendNull());
  ABORT_ON_FAILURE(boolbuilder.Finish(&array_f));

  ABORT_ON_FAILURE(date32builder.AppendValues({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
  ABORT_ON_FAILURE(date32builder.AppendNull());
  ABORT_ON_FAILURE(date32builder.Finish(&array_g));

  ABORT_ON_FAILURE(stringbuilder.AppendValues(
      {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"}));
  ABORT_ON_FAILURE(stringbuilder.AppendNull());
  ABORT_ON_FAILURE(stringbuilder.Finish(&array_h));

  ABORT_ON_FAILURE(tsbuilder.AppendValues({0, 1000000LL * 60LL * 60LL * 24LL, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000, 8000000, 9000000}));
  ABORT_ON_FAILURE(tsbuilder.AppendNull());
  ABORT_ON_FAILURE(tsbuilder.Finish(&array_i));

  auto table =
      arrow::Table::Make(schema, {array_a, array_b, array_c, array_d, array_e,
                                  array_f, array_g, array_h, array_i});

  const char path[] = "example.hyper";
  hyperarrow::arrowTableToHyper(table, path, "schema", "table");

  auto result = hyperarrow::arrowTableFromHyper(path, "schema", "table");
  if (result.ok()) {
    auto read = result.ValueOrDie();
    const int floatIdx = 3;
    // float values do not round trip, so explicitly test
    auto castArray = std::static_pointer_cast<arrow::DoubleArray>(
	read->column(floatIdx)->chunk(0));
    BOOST_TEST(castArray->ApproxEquals(*std::static_pointer_cast<arrow::DoubleArray>(table->column(floatIdx)->chunk(0))));
    auto readResult = read->RemoveColumn(floatIdx);
    auto tableResult = table->RemoveColumn(floatIdx);

    if (!(readResult.ok() && tableResult.ok())) {
      BOOST_ERROR("Could not drop float column from tables");
    } else {
      auto tableNoFloat = tableResult.ValueOrDie();
      auto readNoFloat = readResult.ValueOrDie();
      BOOST_TEST(tableNoFloat->Equals(*readNoFloat));
    }
  } else {
    BOOST_ERROR("Could not read file");
  }
  
  remove(path);
  remove("hyperd.log");
}
