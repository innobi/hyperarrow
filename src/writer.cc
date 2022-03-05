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
#include <hyperarrow/writer.h>

#include <arrow/builder.h>
#include <arrow/compute/api.h>
#include <arrow/table.h>
#include <hyperapi/hyperapi.hpp>

namespace hyperarrow {

class HyperWriterImpl {
public:
  static arrow::Result<std::shared_ptr<HyperWriterImpl>>
  Make(std::shared_ptr<arrow::Schema> schema, const std::string database_path,
       const std::string schema_name, const std::string table_name) {
    auto writer = std::make_shared<HyperWriterImpl>(schema, database_path,
                                                    schema_name, table_name);

    return writer;
  }

  arrow::Status WriteTable(const std::shared_ptr<arrow::Table> table) {
    std::vector<std::function<void(std::shared_ptr<arrow::Array> anArray,
                                   hyperapi::Inserter & inserter,
                                   int64_t colNum, int64_t rowNum)>>
        write_funcs;
    auto schema = table->schema();
    for (auto &field : schema->fields()) {
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
    write_funcs_ = write_funcs;
    arrow::TableBatchReader reader(*table);
    std::shared_ptr<arrow::RecordBatch> batch;
    ARROW_RETURN_NOT_OK(reader.ReadNext(&batch));

    hyperapi::HyperProcess hyper(
        hyperapi::Telemetry::DoNotSendUsageDataToTableau);
    {
      hyperapi::Connection connection(hyper.getEndpoint(), database_path_,
                                      hyperapi::CreateMode::CreateAndReplace);
      const auto &catalog = connection.getCatalog();

      auto extract_table = ConvertSchemaToDefinition();
      catalog.createSchemaIfNotExists(schema_name_);
      catalog.createTable(extract_table);
      {
        hyperapi::Inserter inserter{connection, extract_table};
        while (batch != nullptr) {
          ARROW_RETURN_NOT_OK(this->WriteRecordBatch(*batch, inserter));
        }
        inserter.execute();
      }
    }

    return arrow::Status::OK();
  }

  HyperWriterImpl(std::shared_ptr<arrow::Schema> schema,
                  const std::string database_path,
                  const std::string schema_name, const std::string table_name)
      : schema_(schema), database_path_(database_path),
        schema_name_(schema_name), table_name_(table_name) {}

private:
  hyperapi::TableDefinition ConvertSchemaToDefinition() {
    auto tableDef = hyperapi::TableDefinition({schema_name_, table_name_});
    for (auto field : schema_->fields()) {
      auto name = hyperapi::Name{field->name()};
      auto nullable = field->nullable() ? hyperapi::Nullability::Nullable
                                        : hyperapi::Nullability::NotNullable;
      auto type = hyperarrow::arrowTypeToSqlType(field->type());
      auto col = hyperapi::TableDefinition::Column(name, type, nullable);
      tableDef.addColumn(col);
    }

    return tableDef;
  }

  arrow::Status WriteRecordBatch(const arrow::RecordBatch &batch,
                                 hyperapi::Inserter &inserter) {
    if (batch.num_rows() == 0) {
      return arrow::Status::OK();
    }

    for (int rowNum = 0; rowNum < batch.num_rows(); rowNum++) {
      for (int colNum = 0; colNum < batch.num_columns(); colNum++) {
        auto array = batch.column(colNum);
        write_funcs_[colNum](array, inserter, colNum, rowNum);
      }
      inserter.endRow();
    }
    return arrow::Status::OK();
  }

  const std::shared_ptr<arrow::Schema> schema_;
  const std::string database_path_;
  const std::string schema_name_;
  const std::string table_name_;
  std::vector<std::function<void(std::shared_ptr<arrow::Array> anArray,
                                 hyperapi::Inserter &inserter, int64_t colNum,
                                 int64_t rowNum)>>
      write_funcs_;
};

arrow::Status arrowTableToHyper(const std::shared_ptr<arrow::Table> table,
                                const std::string databasePath,
                                const std::string schemaName,
                                const std::string tableName) {
  {
    ARROW_ASSIGN_OR_RAISE(auto writer,
                          HyperWriterImpl::Make(table->schema(), databasePath,
                                                schemaName, tableName));
    return writer->WriteTable(table);
  }
}
} // namespace hyperarrow
