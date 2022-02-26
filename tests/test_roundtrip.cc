#define BOOST_TEST_MODULE hyperarrow_writer_tests

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

BOOST_AUTO_TEST_CASE(test_basic_roundtrip) {
  auto schema = arrow::schema(
      {arrow::field("a", arrow::int16()), arrow::field("b", arrow::int32()),
       arrow::field("c", arrow::int64()), arrow::field("e", arrow::float64()),
       arrow::field("f", arrow::boolean()), arrow::field("g", arrow::date32()),
       arrow::field("h", arrow::utf8()),
       arrow::field("i", arrow::timestamp(arrow::TimeUnit::MICRO))});

  arrow::MemoryPool *pool = arrow::default_memory_pool();
  arrow::Int16Builder int16builder(pool);
  arrow::Int32Builder int32builder(pool);
  arrow::Int64Builder int64builder(pool);
  arrow::DoubleBuilder doublebuilder(pool);
  arrow::BooleanBuilder boolbuilder(pool);
  arrow::StringBuilder stringbuilder(pool);
  arrow::Date32Builder date32builder(pool);
  arrow::TimestampBuilder tsbuilder(arrow::timestamp(arrow::TimeUnit::MICRO),
                                    pool);
  std::shared_ptr<arrow::Array> array_a;
  std::shared_ptr<arrow::Array> array_b;
  std::shared_ptr<arrow::Array> array_c;
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

  ABORT_ON_FAILURE(tsbuilder.AppendValues(
      {0, 1000000LL * 60LL * 60LL * 24LL, 2000000, 3000000, 4000000, 5000000,
       6000000, 7000000, 8000000, 9000000}));
  ABORT_ON_FAILURE(tsbuilder.AppendNull());
  ABORT_ON_FAILURE(tsbuilder.Finish(&array_i));

  auto table = arrow::Table::Make(schema, {array_a, array_b, array_c, array_e,
                                           array_f, array_g, array_h, array_i});

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


BOOST_AUTO_TEST_CASE(test_roundtrip_batches) {
  const int length = 3;
  
  auto f0 = field("f0", arrow::int8());
  auto f1 = field("f1", arrow::int32());

  std::vector<std::shared_ptr<arrow::Field>> fields = {f0, f1};
  auto schema = arrow::schema(fields);

  arrow::Int8Builder b0;
  arrow::Int32Builder b1;
  std::shared_ptr<arrow::Array> a0, a1;
  ABORT_ON_FAILURE(b0.AppendValues({0, 1, 2}));
  ABORT_ON_FAILURE(b0.Finish(&a0));
  ABORT_ON_FAILURE(b1.AppendValues({100, 101, 102}));
  ABORT_ON_FAILURE(b1.Finish(&a1));
  auto batch0 = arrow::RecordBatch::Make(schema, length, {a0, a1});

  arrow::Int8Builder b2;
  arrow::Int32Builder b3;
  std::shared_ptr<arrow::Array> a2, a3;
  ABORT_ON_FAILURE(b2.AppendValues({3, 4, 5}));
  ABORT_ON_FAILURE(b2.Finish(&a2));
  ABORT_ON_FAILURE(b3.AppendValues({103, 104, 105}));
  ABORT_ON_FAILURE(b3.Finish(&a3));
  auto batch1 = arrow::RecordBatch::Make(schema, length, {a2, a3});

  std::shared_ptr<arrow::Table> table;
  auto res = arrow::Table::FromRecordBatches({batch0, batch1});
  if (res.ok()) {
    table = res.ValueOrDie();
  } else {
    BOOST_ERROR("Could not construct table from batches");
  }

  arrow::Int8Builder eb0;
  arrow::Int32Builder eb1;
  std::shared_ptr<arrow::Array> ea0, ea1;
  ABORT_ON_FAILURE(eb0.AppendValues({0, 1, 2, 3, 4, 5}));
  ABORT_ON_FAILURE(eb0.Finish(&ea0));
  ABORT_ON_FAILURE(eb1.AppendValues({100, 101, 102, 103, 104, 105}));
  ABORT_ON_FAILURE(eb1.Finish(&ea1));
  auto expected = arrow::Table::Make(schema, {ea0, ea1});

  const char path[] = "example.hyper";
  hyperarrow::arrowTableToHyper(table, path, "schema", "table");

  auto result = hyperarrow::arrowTableFromHyper(path, "schema", "table");
  if (result.ok()) {
    auto read = result.ValueOrDie();
    BOOST_TEST(expected->Equals(*read));
  } else {
    BOOST_ERROR("Could not read file");
  }

  remove(path);
  remove("hyperd.log");
  
}
