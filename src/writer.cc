//===-- writer.cc - writer implementation for hyperarrow --------*- C++ -*-===//
//
// Part of the HyperArrow Project, under the Apache License v2.0
// See https://github.com/innobi/hyperarrow/blob/main/LICENSE
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation to writer Arrow tables into Hyper
/// files
///
//===----------------------------------------------------------------------===//

#include "types.h"
#include <arrow/builder.h>
#include <arrow/compute/api.h>
#include <arrow/table.h>
#include <hyperapi/hyperapi.hpp>
#include <hyperarrow/writer.h>
#include <map>

namespace hyperarrow {
static const hyperapi::TableDefinition
createDefinitionFromSchema(std::shared_ptr<arrow::Table> table,
                           const std::string schemaName,
                           const std::string tableName) {
  const std::shared_ptr<arrow::Schema> schema = table->schema();
  hyperapi::TableDefinition tableDef =
      hyperapi::TableDefinition({schemaName, tableName});
  for (const std::shared_ptr<arrow::Field> field : schema->fields()) {
    // TODO: without these conversions can easily get an error like
    // error: no matching function for call to
    // 'hyperapi::TableDefinition::Column::Column(const string&,
    // hyperapi::SqlType, bool)'
    hyperapi::Name name = hyperapi::Name{field->name()};
    hyperapi::Nullability nullable = field->nullable()
                                         ? hyperapi::Nullability::Nullable
                                         : hyperapi::Nullability::NotNullable;
    hyperapi::SqlType type = hyperarrow::arrowTypeToSqlType(field->type());

    hyperapi::TableDefinition::Column col =
        hyperapi::TableDefinition::Column(name, type, nullable);
    tableDef.addColumn(col);
  }

  return tableDef;
}

static const std::vector<std::shared_ptr<arrow::StructArray>>
extractTemporalComponents(const std::shared_ptr<arrow::Table> table) {
  const std::shared_ptr<arrow::Schema> schema = table->schema();
  std::vector<std::shared_ptr<arrow::StructArray>> results;
  results.reserve(schema->num_fields());

  for (int i = 0; i < schema->num_fields(); i++) {
    auto type = schema->field(i)->type()->id();
    if (type == arrow::Type::TIMESTAMP || type == arrow::Type::DATE32) {
      std::vector<std::shared_ptr<arrow::Array>> result;
      std::vector<std::string> functions;
      if (type == arrow::Type::TIMESTAMP) {
        auto array = std::static_pointer_cast<arrow::TimestampArray>(
            table->column(i)->chunk(0));

        functions = {"year",   "month",  "day",        "hour",
                     "minute", "second", "microsecond"};
        for (auto function : functions) {
          auto res =
              arrow::compute::CallFunction(function, {array}).ValueOrDie();
          result.push_back(res.make_array());
        }
      } else if (type == arrow::Type::DATE32) {
        auto array = std::static_pointer_cast<arrow::Date32Array>(
            table->column(i)->chunk(0));

        functions = {"year", "month", "day"};
        for (auto function : functions) {
          auto res =
              arrow::compute::CallFunction(function, {array}).ValueOrDie();
          result.push_back(res.make_array());
        }
      }
      results.push_back(
          arrow::StructArray::Make(result, functions).ValueOrDie());
    } else {
      results.push_back(NULL);
    }
  }

  return results;
}

void arrowTableToHyper(const std::shared_ptr<arrow::Table> table,
                       const std::string databasePath,
                       const std::string schemaName,
                       const std::string tableName) {
  {
    auto temporalComponents = extractTemporalComponents(table);

    std::vector<std::function<void(std::shared_ptr<arrow::Array> anArray,
                                   hyperapi::Inserter & inserter,
                                   int64_t colNum, int64_t rowNum)>>
        write_funcs;
    for (auto &field : table->fields()) {
      auto type_id = field->type()->id();
      if (type_id == arrow::Type::INT16) {
        write_funcs.push_back([](std::shared_ptr<arrow::Array> anArray,
                                 hyperapi::Inserter &inserter, int64_t colNum,
                                 int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::Int16Array>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->Value(rowNum));
          } else {
            inserter.add(hyperapi::optional<int16_t>());
          }
        });
      } else if (type_id == arrow::Type::INT32) {
        write_funcs.push_back([](std::shared_ptr<arrow::Array> anArray,
                                 hyperapi::Inserter &inserter, int64_t colNum,
                                 int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::Int32Array>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->Value(rowNum));
          } else {
            inserter.add(hyperapi::optional<int32_t>());
          }
        });
      } else if (type_id == arrow::Type::INT64) {
        write_funcs.push_back([](std::shared_ptr<arrow::Array> anArray,
                                 hyperapi::Inserter &inserter, int64_t colNum,
                                 int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::Int64Array>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->Value(rowNum));
          } else {
            inserter.add(hyperapi::optional<int64_t>());
          }
        });
      } else if (type_id == arrow::Type::DOUBLE) {
        write_funcs.push_back([](std::shared_ptr<arrow::Array> anArray,
                                 hyperapi::Inserter &inserter, int64_t colNum,
                                 int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::DoubleArray>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->Value(rowNum));
          } else {
            inserter.add(hyperapi::optional<double_t>());
          }
        });
      } else if (type_id == arrow::Type::BOOL) {
        write_funcs.push_back([](std::shared_ptr<arrow::Array> anArray,
                                 hyperapi::Inserter &inserter, int64_t colNum,
                                 int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::BooleanArray>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->Value(rowNum));
          } else {
            inserter.add(hyperapi::optional<bool>());
          }
        });
      } else if (type_id == arrow::Type::DATE32) {
        write_funcs.push_back([temporalComponents](
                                  std::shared_ptr<arrow::Array> anArray,
                                  hyperapi::Inserter &inserter, int64_t colNum,
                                  int64_t rowNum) {
          auto array = temporalComponents[colNum];
          auto flattened = array->Flatten().ValueOrDie();
          auto yearArr =
              std::static_pointer_cast<arrow::Int64Array>(flattened[0]);
          if (yearArr->IsValid(rowNum)) {
            auto year = yearArr->Value(rowNum);
            auto month =
                std::static_pointer_cast<arrow::Int64Array>(flattened[1])
                    ->Value(rowNum);
            auto day = std::static_pointer_cast<arrow::Int64Array>(flattened[2])
                           ->Value(rowNum);
            auto date = hyperapi::Date(year, month, day);
            inserter.add(date);
          } else {
            inserter.add(hyperapi::optional<hyperapi::Date>());
          }
        });
      } else if (type_id == arrow::Type::TIMESTAMP) {
        write_funcs.push_back([temporalComponents](
                                  std::shared_ptr<arrow::Array> anArray,
                                  hyperapi::Inserter &inserter, int64_t colNum,
                                  int64_t rowNum) {
          auto array = temporalComponents[colNum];
          auto flattened = array->Flatten().ValueOrDie();
          auto yearArr =
              std::static_pointer_cast<arrow::Int64Array>(flattened[0]);
          if (yearArr->IsValid(rowNum)) {
            auto year = yearArr->Value(rowNum);
            auto month =
                std::static_pointer_cast<arrow::Int64Array>(flattened[1])
                    ->Value(rowNum);
            auto day = std::static_pointer_cast<arrow::Int64Array>(flattened[2])
                           ->Value(rowNum);
            auto hour =
                std::static_pointer_cast<arrow::Int64Array>(flattened[3])
                    ->Value(rowNum);
            auto minute =
                std::static_pointer_cast<arrow::Int64Array>(flattened[4])
                    ->Value(rowNum);
            auto second =
                std::static_pointer_cast<arrow::Int64Array>(flattened[5])
                    ->Value(rowNum);
            auto microsecond =
                std::static_pointer_cast<arrow::Int64Array>(flattened[6])
                    ->Value(rowNum);
            auto time = hyperapi::Time(hour, minute, second, microsecond);
            auto date = hyperapi::Date(year, month, day);
            inserter.add(hyperapi::Timestamp(date, time));
          } else {
            inserter.add(hyperapi::optional<hyperapi::Timestamp>());
          }
        });
      } else if (type_id == arrow::Type::STRING) {
        write_funcs.push_back([](std::shared_ptr<arrow::Array> anArray,
                                 hyperapi::Inserter &inserter, int64_t colNum,
                                 int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::StringArray>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->GetString(rowNum));
          } else {
            inserter.add(hyperapi::optional<std::string>());
          }
        });
      }
    }

    hyperapi::HyperProcess hyper(
        hyperapi::Telemetry::DoNotSendUsageDataToTableau);
    {
      hyperapi::Connection connection(hyper.getEndpoint(), databasePath,
                                      hyperapi::CreateMode::CreateAndReplace);
      const hyperapi::Catalog &catalog = connection.getCatalog();
      const hyperapi::TableDefinition extractTable =
          createDefinitionFromSchema(table, schemaName, tableName);

      catalog.createSchemaIfNotExists(schemaName);
      catalog.createTable(extractTable);
      {
        hyperapi::Inserter inserter{connection, extractTable};
        for (int64_t rowNum = 0; rowNum < table->num_rows(); rowNum++) {
          for (int64_t colNum = 0; colNum < table->num_columns(); colNum++) {
            auto chunk = table->column(colNum)->chunk(0);
            write_funcs[colNum](chunk, inserter, colNum, rowNum);
          }
          inserter.endRow();
        }
        inserter.execute();
      }
    }
  }
}
} // namespace hyperarrow
