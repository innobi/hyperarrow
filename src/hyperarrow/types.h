#include <hyperapi/hyperapi.hpp>
#include <arrow/table.h>

namespace hyperarrow {
  hyperapi::SqlType arrowTypeToSqlType(const std::shared_ptr<arrow::DataType> arrowType);
}
