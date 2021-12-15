#include "types.h"

#include <stdexcept>

namespace hyperarrow {
hyperapi::SqlType
arrowTypeToSqlType(const std::shared_ptr<arrow::DataType> arrowType) {
  arrow::Type::type type_id = arrowType->id();
  if (type_id == arrow::Type::INT16) {
    return hyperapi::SqlType::smallInt();
  } else if (type_id == arrow::Type::INT32) {
    return hyperapi::SqlType::integer();
  } else if (type_id == arrow::Type::INT64) {
    return hyperapi::SqlType::bigInt();
  } else if (type_id == arrow::Type::DOUBLE) {
    return hyperapi::SqlType::doublePrecision();
  } else if (type_id == arrow::Type::BOOL) {
    return hyperapi::SqlType::boolean();
  } else if (type_id == arrow::Type::STRING) {
    return hyperapi::SqlType::text();
  } else if (type_id == arrow::Type::DATE32) {
    return hyperapi::SqlType::date();
  } else if (type_id == arrow::Type::TIMESTAMP) {
    return hyperapi::SqlType::timestamp();
  }

  throw std::runtime_error(
      std::string("Type not supported or not yet implemented: ") +
      typeid(type_id).name());
}

std::shared_ptr<arrow::DataType>
hyperTypeToArrowType(const hyperapi::SqlType hyperType) {
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
  } else if (hyperType == hyperapi::SqlType::timestamp()) {
    return arrow::timestamp(arrow::TimeUnit::MICRO);
  }

  throw std::runtime_error(
      std::string("type not supported or not yet implemented: ") +
      typeid(hyperType).name());
}
} // namespace hyperarrow
