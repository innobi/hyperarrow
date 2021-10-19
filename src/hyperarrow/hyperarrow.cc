#include <iostream>

#include <hyperapi/hyperapi.hpp>
#include <arrow/builder.h>
#include <arrow/table.h>

#include "writer.h"

using arrow::Status;

#define ABORT_ON_FAILURE(expr)                     \
  do {                                             \
    arrow::Status status_ = (expr);                \
    if (!status_.ok()) {                           \
      std::cerr << status_.message() << std::endl; \
      abort();                                     \
    }                                              \
  } while (0);


namespace {

  std::shared_ptr<arrow::Table> createTable() {
    auto schema =
      arrow::schema({
	  arrow::field("a", arrow::int64()),
	  arrow::field("b", arrow::int64()),
	  arrow::field("c", arrow::int64())
	});
    std::shared_ptr<arrow::Array> array_a;
    std::shared_ptr<arrow::Array> array_b;
    std::shared_ptr<arrow::Array> array_c;
    arrow::NumericBuilder<arrow::Int64Type> builder;
    ABORT_ON_FAILURE(builder.AppendValues({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    ABORT_ON_FAILURE(builder.AppendNull());
    ABORT_ON_FAILURE(builder.Finish(&array_a));
    builder.Reset();
    ABORT_ON_FAILURE(builder.AppendValues({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
    ABORT_ON_FAILURE(builder.AppendNull());
    ABORT_ON_FAILURE(builder.Finish(&array_b));
    builder.Reset();
    ABORT_ON_FAILURE(builder.AppendValues({1, 2, 1, 2, 1, 2, 1, 2, 1, 2}));
    ABORT_ON_FAILURE(builder.AppendNull());
    ABORT_ON_FAILURE(builder.Finish(&array_c));
    return arrow::Table::Make(schema, {array_a, array_b, array_c});
  }

  Status RunMain(int argc, char** argv) {
    std::cerr << "* Generating data:" << std::endl;
    std::shared_ptr<arrow::Table> table = createTable();  

    std::cerr << "* Creating Hyper File:" << std::endl;
    hyperarrow::arrowTableToHyper(table, "example.hyper");
    std::cerr << "* Hyper File Created Successfullly!" << std::endl;

    return Status::OK();
  }

}  // namespace

int main(int argc, char** argv) {
  Status st = RunMain(argc, argv);
  if (!st.ok()) {
    std::cerr << st << std::endl;
    return 1;
  }
  return 0;
}
