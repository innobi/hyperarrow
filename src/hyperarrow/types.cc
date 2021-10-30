#include "types.h"

namespace hyperarrow {
  hyperapi::SqlType arrowTypeToSqlType(const std::shared_ptr<arrow::DataType> arrowType) {
    if (arrowType == arrow::int16()) {
      return hyperapi::SqlType::smallInt();
    } else if (arrowType == arrow::int32())  {
      return  hyperapi::SqlType::integer();
    } else if (arrowType == arrow::int64())
      return hyperapi::SqlType::bigInt();
  }
}
