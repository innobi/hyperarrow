#include <arrow/builder.h>
#include <arrow/table.h>

#include <hyperapi/hyperapi.hpp>
#include <iostream>

#include "hyperarrow/writer.h"

using arrow::Status;

#define ABORT_ON_FAILURE(expr)                                                 \
  do {                                                                         \
    arrow::Status status_ = (expr);                                            \
    if (!status_.ok()) {                                                       \
      std::cerr << status_.message() << std::endl;                             \
      abort();                                                                 \
    }                                                                          \
  } while (0);

namespace {

std::shared_ptr<arrow::Table> createTable() {
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

  ABORT_ON_FAILURE(int16builder.Append(16));
  ABORT_ON_FAILURE(int16builder.AppendNull());
  ABORT_ON_FAILURE(int16builder.Finish(&array_a));

  ABORT_ON_FAILURE(int32builder.Append(32));
  ABORT_ON_FAILURE(int32builder.AppendNull());
  ABORT_ON_FAILURE(int32builder.Finish(&array_b));

  ABORT_ON_FAILURE(int64builder.Append(64));
  ABORT_ON_FAILURE(int64builder.AppendNull());
  ABORT_ON_FAILURE(int64builder.Finish(&array_c));

  ABORT_ON_FAILURE(floatbuilder.Append(1.234));
  ABORT_ON_FAILURE(floatbuilder.AppendNull());
  ABORT_ON_FAILURE(floatbuilder.Finish(&array_d));

  ABORT_ON_FAILURE(doublebuilder.Append(123.4));
  ABORT_ON_FAILURE(doublebuilder.AppendNull());
  ABORT_ON_FAILURE(doublebuilder.Finish(&array_e));

  ABORT_ON_FAILURE(boolbuilder.Append(true));
  ABORT_ON_FAILURE(boolbuilder.AppendNull());
  ABORT_ON_FAILURE(boolbuilder.Finish(&array_f));

  ABORT_ON_FAILURE(date32builder.Append(0));
  ABORT_ON_FAILURE(date32builder.AppendNull());
  ABORT_ON_FAILURE(date32builder.Finish(&array_g));

  ABORT_ON_FAILURE(stringbuilder.Append("a"));
  ABORT_ON_FAILURE(stringbuilder.AppendNull());
  ABORT_ON_FAILURE(stringbuilder.Finish(&array_h));

  ABORT_ON_FAILURE(tsbuilder.Append(0));
  ABORT_ON_FAILURE(tsbuilder.AppendNull());
  ABORT_ON_FAILURE(tsbuilder.Finish(&array_i));

  return arrow::Table::Make(schema,
                            {array_a, array_b, array_c, array_d, array_e,
                             array_f, array_g, array_h, array_i});
}

Status RunMain(int argc, char **argv) {
  std::cerr << "* Generating data:" << std::endl;
  std::shared_ptr<arrow::Table> table = createTable();

  std::cerr << "* Creating Hyper File:" << std::endl;
  hyperarrow::arrowTableToHyper(table, "example.hyper", "schema", "table");
  std::cerr << "* Hyper File Created Successfullly!" << std::endl;

  return Status::OK();
}

} // namespace

int main(int argc, char **argv) {
  Status st = RunMain(argc, argv);
  if (!st.ok()) {
    std::cerr << st << std::endl;
    return 1;
  }
  return 0;
}
