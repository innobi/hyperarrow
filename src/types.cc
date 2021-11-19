#include "types.h"

#include <stdexcept>

namespace hyperarrow {
hyperapi::SqlType arrowTypeToSqlType(
    const std::shared_ptr<arrow::DataType> arrowType) {
  if (arrowType == arrow::int16()) {
    return hyperapi::SqlType::smallInt();
  } else if (arrowType == arrow::int32()) {
    return hyperapi::SqlType::integer();
  } else if (arrowType == arrow::int64()) {
    return hyperapi::SqlType::bigInt();
  } else if (arrowType == arrow::float32()) {
    return hyperapi::SqlType::doublePrecision();
  } else if (arrowType == arrow::float64()) {
    return hyperapi::SqlType::doublePrecision();
  } else if (arrowType == arrow::boolean()) {
    return hyperapi::SqlType::boolean();
  } else if (arrowType == arrow::utf8()) {
    return hyperapi::SqlType::text();
  } else if (arrowType == arrow::date32()) {
    return hyperapi::SqlType::date();
  }

  throw std::runtime_error(
      std::string("Type not supported or not yet implemented: ") +
      typeid(arrowType).name());
}

  std::shared_ptr<arrow::DataType> hyperTypeToArrowType(const hyperapi::SqlType hyperType) {
    if (hyperType == hyperapi::SqlType::smallInt()) {
      return arrow::int16();
    } else if (hyperType == hyperapi::SqlType::integer()) {
      return arrow::int32();
    } else if (hyperType == hyperapi::SqlType::bigInt()) {
      return arrow::int64();
    } else if (hyperType == hyperapi::SqlType::doublePrecision()) {
      return arrow::float64();
    } else if (hyperType == hyperapi::SqlType::boolean()) {
      return arrow::boolean();
    } else if (hyperType == hyperapi::SqlType::text()) {
      return arrow::utf8();
    } else if (hyperType == hyperapi::SqlType::date()) {
      return arrow::date32();
    }

    throw std::runtime_error(
	std::string("type note supported or not yet implemented: ") +
	typeid(hyperType).name());
  }
}  // namespace hyperarrow
