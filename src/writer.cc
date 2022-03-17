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
#include <stdexcept>

namespace hyperarrow {

  class BasePopulator {
  public:
    virtual void Insert(hyperapi::Inserter &inserter) {};

  protected:
    size_t rowNumber_ = 0;
  };

  class Int16Populator : public BasePopulator {
  public:
    Int16Populator(std::shared_ptr<arrow::Array> array) {
      array_ = std::static_pointer_cast<arrow::Int16Array>(array);
    }

    void Insert(hyperapi::Inserter &inserter) override {
      if (array_->IsValid(rowNumber_)) {
	inserter.add(array_->Value(rowNumber_));
      } else {
	inserter.add(hyperapi::optional<int16_t>());
      }

      rowNumber_++;
    }

  private:
    std::shared_ptr<arrow::Int16Array> array_;
  };

  class Int32Populator : public BasePopulator {
  public:
    Int32Populator(std::shared_ptr<arrow::Array> array) {
      array_ = std::static_pointer_cast<arrow::Int32Array>(array);
    }

    void Insert(hyperapi::Inserter &inserter) override {
      if (array_->IsValid(rowNumber_)) {
	inserter.add(array_->Value(rowNumber_));
      } else {
	inserter.add(hyperapi::optional<int32_t>());
      }
      rowNumber_++;
    }

  private:
    std::shared_ptr<arrow::Int32Array> array_;
  };

  class Int64Populator : public BasePopulator {
  public:
    Int64Populator(std::shared_ptr<arrow::Array> array) {
      array_ = std::static_pointer_cast<arrow::Int64Array>(array);
    }

    void Insert(hyperapi::Inserter &inserter) override {
      if (array_->IsValid(rowNumber_)) {
	inserter.add(array_->Value(rowNumber_));
      } else {
	inserter.add(hyperapi::optional<int64_t>());
      }
      rowNumber_++;
    }

  private:
    std::shared_ptr<arrow::Int64Array> array_;
  };

  class DoublePopulator : public BasePopulator {
  public:
    DoublePopulator(std::shared_ptr<arrow::Array> array) {
      array_ = std::static_pointer_cast<arrow::DoubleArray>(array);
    }

    void Insert(hyperapi::Inserter &inserter) override {
      if (array_->IsValid(rowNumber_)) {
	inserter.add(array_->Value(rowNumber_));
      } else {
	inserter.add(hyperapi::optional<double_t>());
      }
      rowNumber_++;
    }

  private:
    std::shared_ptr<arrow::DoubleArray> array_;
  };

  class BooleanPopulator : public BasePopulator {
  public:
    BooleanPopulator(std::shared_ptr<arrow::Array> array) {
      array_ = std::static_pointer_cast<arrow::BooleanArray>(array);
    }

    void Insert(hyperapi::Inserter &inserter) override {
      if (array_->IsValid(rowNumber_)) {
	inserter.add(array_->Value(rowNumber_));
      } else {
	inserter.add(hyperapi::optional<bool>());
      }
      rowNumber_++;
    }

  private:
    std::shared_ptr<arrow::BooleanArray> array_;
  };

  class StringPopulator : public BasePopulator {
  public:
    StringPopulator(std::shared_ptr<arrow::Array> array) {
      array_ = std::static_pointer_cast<arrow::StringArray>(array);
    }

    void Insert(hyperapi::Inserter &inserter) override {
      if (array_->IsValid(rowNumber_)) {
	inserter.add(array_->GetString(rowNumber_));
      } else {
	inserter.add(hyperapi::optional<std::string>());
      }
      rowNumber_++;
    }

  private:
    std::shared_ptr<arrow::StringArray> array_;
  };

  class Date32Populator : public BasePopulator {
  public:
    Date32Populator(std::shared_ptr<arrow::Array> array) {
      auto tempArray = std::static_pointer_cast<arrow::Date32Array>(array);
      auto result = arrow::compute::CallFunction("year_month_day", {tempArray});
      arrow::Datum datum;
      if (result.ok()) {
	datum = result.ValueOrDie();
      } else {
	throw std::runtime_error("year_month_day call failed");
      }
      
      array_ = std::static_pointer_cast<arrow::StructArray>(datum.make_array());
      years_ = std::static_pointer_cast<arrow::Int64Array>(array_->GetFieldByName("year"));
      months_ = std::static_pointer_cast<arrow::Int64Array>(array_->GetFieldByName("month"));
      days_ = std::static_pointer_cast<arrow::Int64Array>(array_->GetFieldByName("day"));
    }

    void Insert(hyperapi::Inserter &inserter) override {
      if (array_->IsValid(rowNumber_)) {
	auto year = years_->Value(rowNumber_);
	auto month = months_->Value(rowNumber_);
	auto day = days_->Value(rowNumber_);
	auto date = hyperapi::Date(year, month, day);
	inserter.add(date);
      } else {
	inserter.add(hyperapi::optional<hyperapi::Date>());
      }
      rowNumber_++;
    }

  private:
    std::shared_ptr<arrow::StructArray> array_;
    std::shared_ptr<arrow::Int64Array> years_;
    std::shared_ptr<arrow::Int64Array> months_;
    std::shared_ptr<arrow::Int64Array> days_;

  };

  class TimestampPopulator : public BasePopulator {
  public:
    TimestampPopulator(std::shared_ptr<arrow::Array> array) {
      auto tempArray = std::static_pointer_cast<arrow::TimestampArray>(array);
      auto result = arrow::compute::CallFunction("year_month_day", {tempArray});
      arrow::Datum datum;
      if (result.ok()) {
	datum = result.ValueOrDie();
      } else {
	throw std::runtime_error("year_month_day call failed");
      }

      array_ = std::static_pointer_cast<arrow::StructArray>(datum.make_array());
      years_ = std::static_pointer_cast<arrow::Int64Array>(array_->GetFieldByName("year"));
      months_ = std::static_pointer_cast<arrow::Int64Array>(array_->GetFieldByName("month"));
      days_ = std::static_pointer_cast<arrow::Int64Array>(array_->GetFieldByName("day"));


      auto hourResult = arrow::compute::CallFunction("hour", {tempArray});
      if (hourResult.ok()) {
	datum = hourResult.ValueOrDie();
      } else {
	throw std::runtime_error("hour call failed");
      }
      hours_ = std::static_pointer_cast<arrow::Int64Array>(datum.make_array());

      auto minuteResult = arrow::compute::CallFunction("minute", {tempArray});
      if (minuteResult.ok()) {
	datum = minuteResult.ValueOrDie();
      } else {
	throw std::runtime_error("minute call failed");
      }
      minutes_ = std::static_pointer_cast<arrow::Int64Array>(datum.make_array());

      auto secondResult = arrow::compute::CallFunction("second", {tempArray});
      if (secondResult.ok()) {
	datum = secondResult.ValueOrDie();
      } else {
        throw std::runtime_error("second call failed");
      }
      seconds_ = std::static_pointer_cast<arrow::Int64Array>(datum.make_array());

      auto microsecondResult = arrow::compute::CallFunction("microsecond", {tempArray});
      if (microsecondResult.ok()) {
	datum = microsecondResult.ValueOrDie();
      } else {
	throw std::runtime_error("microsecond call failed");
      }
      microseconds_ = std::static_pointer_cast<arrow::Int64Array>(datum.make_array());      
            
    }

    void Insert(hyperapi::Inserter &inserter) override {
      if (array_->IsValid(rowNumber_)) {
	auto year = years_->Value(rowNumber_);
	auto month = months_->Value(rowNumber_);
	auto day = days_->Value(rowNumber_);
	auto date = hyperapi::Date(year, month, day);

	auto hour = hours_->Value(rowNumber_);
	auto minute = minutes_->Value(rowNumber_);
	auto second = seconds_->Value(rowNumber_);
	auto microsecond = microseconds_->Value(rowNumber_);
	auto time = hyperapi::Time(hour, minute, second, microsecond);

	auto timestamp = hyperapi::Timestamp(date, time);
	inserter.add(timestamp);
      } else {
	inserter.add(hyperapi::optional<hyperapi::Timestamp>());
      }
      rowNumber_++;
    }

  private:
    std::shared_ptr<arrow::StructArray> array_;
    std::shared_ptr<arrow::Int64Array> years_;
    std::shared_ptr<arrow::Int64Array> months_;
    std::shared_ptr<arrow::Int64Array> days_;
    std::shared_ptr<arrow::Int64Array> hours_;
    std::shared_ptr<arrow::Int64Array> minutes_;
    std::shared_ptr<arrow::Int64Array> seconds_;
    std::shared_ptr<arrow::Int64Array> microseconds_;

  };  
  
  

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
	  reader.ReadNext(&batch);
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

    std::vector<std::shared_ptr<BasePopulator>> populators;
    auto schema = batch.schema();
    for (int colNum = 0; colNum < batch.num_columns(); colNum++) {
      auto field = batch.schema()->field(colNum);
      auto type_id = field->type()->id();
      auto array = batch.column(colNum);
      if (type_id == arrow::Type::INT16) {
	populators.push_back(std::make_shared<Int16Populator>(array));
      } else if (type_id == arrow::Type::INT32) {
	populators.push_back(std::make_shared<Int32Populator>(array));
      } else if (type_id == arrow::Type::INT64) {
	populators.push_back(std::make_shared<Int64Populator>(array));	
      } else if (type_id == arrow::Type::DOUBLE) {
	populators.push_back(std::make_shared<DoublePopulator>(array));
      } else if (type_id == arrow::Type::BOOL) {
	populators.push_back(std::make_shared<BooleanPopulator>(array));
      } else if (type_id == arrow::Type::STRING) {
	populators.push_back(std::make_shared<StringPopulator>(array));
      } else if (type_id == arrow::Type::DATE32) {
	populators.push_back(std::make_shared<Date32Populator>(array));
      } else if (type_id == arrow::Type::TIMESTAMP) {
	populators.push_back(std::make_shared<TimestampPopulator>(array));
      }
    }
    
    for (int rowNum = 0; rowNum < batch.num_rows(); rowNum++) {
      for (auto &populator : populators) {
	populator->Insert(inserter);
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
