#include "types.h"

namespace hyperarrow {
  hyperapi::SqlType arrowTypeToSqlType(const std::shared_ptr<arrow::DataType> arrowType) {
    if (arrowType == arrow::int64())
      return hyperapi::SqlType::bigInt();
  }
}
