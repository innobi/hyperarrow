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
	  arrow::field("b", arrow::int32()),
	  arrow::field("c", arrow::int64()),
	  //arrow::field("d", arrow::float16()),
	  arrow::field("e", arrow::float32()),
	  arrow::field("f", arrow::float64()),
	  arrow::field("g", arrow::boolean())
	});

    arrow::MemoryPool *pool = arrow::default_memory_pool();
    arrow::Int16Builder int16builder(pool);
    arrow::Int32Builder int32builder(pool);
    arrow::Int64Builder int64builder(pool);
    arrow::HalfFloatBuilder hfbuilder(pool);
    arrow::FloatBuilder floatbuilder(pool);
    arrow::DoubleBuilder doublebuilder(pool);
    arrow::BooleanBuilder boolbuilder(pool);
    std::shared_ptr<arrow::Array> array_a;
    std::shared_ptr<arrow::Array> array_b;
    std::shared_ptr<arrow::Array> array_c;
    std::shared_ptr<arrow::Array> array_d;
    std::shared_ptr<arrow::Array> array_e;
    std::shared_ptr<arrow::Array> array_f;
    std::shared_ptr<arrow::Array> array_g;
    
    ABORT_ON_FAILURE(int16builder.AppendValues({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    ABORT_ON_FAILURE(int16builder.AppendNull());
    ABORT_ON_FAILURE(int16builder.Finish(&array_a));
    //int16builder.Reset();
    ABORT_ON_FAILURE(int32builder.AppendValues({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
    ABORT_ON_FAILURE(int32builder.AppendNull());
    ABORT_ON_FAILURE(int32builder.Finish(&array_b));
    //int32builder.Reset();
    ABORT_ON_FAILURE(int64builder.AppendValues({1, 2, 1, 2, 1, 2, 1, 2, 1, 2}));
    ABORT_ON_FAILURE(int64builder.AppendNull());
    ABORT_ON_FAILURE(int64builder.Finish(&array_c));

    /*
    ABORT_ON_FAILURE(hfbuilder.AppendValues({0., 1., 2., 3., 4., 5., 6., 7., 8., 9.}));
    ABORT_ON_FAILURE(hfbuilder.AppendNull());
    ABORT_ON_FAILURE(hfbuilder.Finish(&array_d));
    */

    ABORT_ON_FAILURE(floatbuilder.AppendValues({0., 1., 2., 3., 4., 5., 6., 7., 8., 9.}));
    ABORT_ON_FAILURE(floatbuilder.AppendNull());
    ABORT_ON_FAILURE(floatbuilder.Finish(&array_e));

    ABORT_ON_FAILURE(doublebuilder.AppendValues({0., 1., 2., 3., 4., 5., 6., 7., 8., 9.}));
    ABORT_ON_FAILURE(doublebuilder.AppendNull());
    ABORT_ON_FAILURE(doublebuilder.Finish(&array_f));

    ABORT_ON_FAILURE(boolbuilder.AppendValues({true, false, true, false, true, false, true, false, true, false}));
    ABORT_ON_FAILURE(boolbuilder.AppendNull());
    ABORT_ON_FAILURE(boolbuilder.Finish(&array_g));

    return arrow::Table::Make(schema, {
	array_a,
	array_b,
	array_c,
	//array_d,
	array_e,
	array_f,
	array_g});
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
