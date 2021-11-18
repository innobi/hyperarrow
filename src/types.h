#include <arrow/table.h>

#include <hyperapi/hyperapi.hpp>

namespace hyperarrow {
hyperapi::SqlType arrowTypeToSqlType(
    const std::shared_ptr<arrow::DataType> arrowType);

  std::shared_ptr<arrow::DataType> hyperTypeToArrowType(const hyperapi::SqlType hyperType);
}
