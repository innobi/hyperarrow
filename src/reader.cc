#include "hyperarrow/reader.h"
#include "types.h"
#include <arrow/api.h>
#include <arrow/vendored/datetime.h>
#include <hyperapi/hyperapi.hpp>
#include <vector>

namespace hyperarrow {
static std::shared_ptr<arrow::Schema>
schemaFromHyper(hyperapi::TableDefinition tableDefinition) {
  std::vector<std::shared_ptr<arrow::Field>> fields;
  for (auto &column : tableDefinition.getColumns()) {
    auto type = hyperarrow::hyperTypeToArrowType(column.getType());
    auto field = arrow::field(column.getName().getUnescaped(), type);
    fields.push_back(field);
  }

  std::shared_ptr<arrow::Schema> schema;

  schema = arrow::schema(fields);

  return schema;
}

arrow::Result<std::shared_ptr<arrow::Table>>
arrowTableFromHyper(const std::string databasePath,
                    const std::string schemaName, const std::string tableName) {
  std::size_t colCount;
  std::vector<std::shared_ptr<arrow::ArrayBuilder>> builders;
  std::shared_ptr<arrow::Schema> schema;

  hyperapi::HyperProcess hyper(
      hyperapi::Telemetry::DoNotSendUsageDataToTableau);
  {
    hyperapi::Connection connection(hyper.getEndpoint(), databasePath);
    const hyperapi::Catalog &catalog = connection.getCatalog();
    hyperapi::TableName extractTable(schemaName, tableName);

    hyperapi::TableDefinition tableDefinition =
        catalog.getTableDefinition(extractTable);
    schema = schemaFromHyper(tableDefinition);
    colCount = schema->num_fields();

    std::vector<std::function<arrow::Status(const hyperapi::Value &value)>>
        append_funcs;
    for (int i = 0; i < schema->fields().size(); i++) {
      const auto &field = schema->fields()[i];
      if (schema->field(i)->type() == arrow::int16()) {
        auto builder = std::make_shared<arrow::Int16Builder>();
        append_funcs.push_back([builder](const hyperapi::Value &value) {
          if (value.isNull()) {
            return builder->AppendNull();
          } else {
            return builder->Append(value);
          }
        });
        // TODO: if we get rowCount up front we can more efficiently append
        // builder->Reserve(rowCount);
        builders.push_back(std::move(builder));
      } else if (schema->field(i)->type() == arrow::int32()) {
        auto builder = std::make_shared<arrow::Int32Builder>();
        append_funcs.push_back([builder](const hyperapi::Value &value) {
          if (value.isNull()) {
            return builder->AppendNull();
          } else {
            return builder->Append(value);
          }
        });

        // TODO: if we get rowCount up front we can more efficiently append
        // builder->Reserve(rowCount);
        builders.push_back(std::move(builder));
      } else if (schema->field(i)->type() == arrow::int64()) {
        auto builder = std::make_shared<arrow::Int64Builder>();
        append_funcs.push_back([builder](const hyperapi::Value &value) {
          if (value.isNull()) {
            return builder->AppendNull();
          } else {
            return builder->Append(value);
          }
        });
        builders.push_back(std::move(builder));
      } else if (schema->field(i)->type() == arrow::float64()) {
        auto builder = std::make_shared<arrow::DoubleBuilder>();
        append_funcs.push_back([builder](const hyperapi::Value &value) {
          if (value.isNull()) {
            return builder->AppendNull();
          } else {
            return builder->Append(value);
          }
        });
        builders.push_back(std::move(builder));
      } else if (schema->field(i)->type() == arrow::boolean()) {
        auto builder = std::make_shared<arrow::BooleanBuilder>();
        append_funcs.push_back([builder](const hyperapi::Value &value) {
          if (value.isNull()) {
            return builder->AppendNull();
          } else {
            return builder->Append(static_cast<bool>(value));
          }
        });
        builders.push_back(std::move(builder));
      } else if (schema->field(i)->type() == arrow::utf8()) {
        auto builder = std::make_shared<arrow::StringBuilder>();
        append_funcs.push_back([builder](const hyperapi::Value &value) {
          if (value.isNull()) {
            return builder->AppendNull();
          } else {
            std::string stringVal = value.get<std::string>();
            return builder->Append(stringVal);
          }
        });
        builders.push_back(std::move(builder));
      } else if (schema->field(i)->type() == arrow::date32()) {
        auto builder = std::make_shared<arrow::Date32Builder>();
        append_funcs.push_back([builder](const hyperapi::Value &value) {
          if (value.isNull()) {
            return builder->AppendNull();
          } else {
            hyperapi::Date dt = value.get<hyperapi::Date>();
            // Arrow uses Unix epoch
            auto chrono_dt = arrow_vendored::date::sys_days(
                arrow_vendored::date::year(dt.getYear()) /
                arrow_vendored::date::month(dt.getMonth()) /
                arrow_vendored::date::day(dt.getDay()));
            auto epoch = chrono_dt.time_since_epoch();
            auto val =
                std::chrono::duration_cast<arrow_vendored::date::days>(epoch);
            return builder->Append(val.count());
          }
        });
        builders.push_back(std::move(builder));
      } else if (schema->field(i)->type()->id() == arrow::Type::TIMESTAMP) {
        // TODO: specialization fails withotu providing
        // arrow::default_memory_pool() as the second argument. Seems like an
        // arrow bug
        auto builder = std::make_shared<arrow::TimestampBuilder>(
            arrow::timestamp(arrow::TimeUnit::MICRO),
            arrow::default_memory_pool());
        append_funcs.push_back([builder](const hyperapi::Value &value) {
          if (value.isNull()) {
            return builder->AppendNull();
          } else {
            auto ts = value.get<hyperapi::Timestamp>();
            auto dt = ts.getDate();
            auto time = ts.getTime();
            // Arrow uses Unix epoch
            auto chrono_time = arrow_vendored::date::sys_days(
                                   arrow_vendored::date::year(dt.getYear()) /
                                   arrow_vendored::date::month(dt.getMonth()) /
                                   arrow_vendored::date::day(dt.getDay())) +
                               std::chrono::hours(time.getHour()) +
                               std::chrono::minutes(time.getMinute()) +
                               std::chrono::seconds(time.getSecond()) +
                               std::chrono::microseconds(time.getMicrosecond());
            auto epoch = chrono_time.time_since_epoch();
            auto val =
                std::chrono::duration_cast<std::chrono::microseconds>(epoch);
            return builder->Append(val.count());
          }
        });
        builders.push_back(std::move(builder));
      }
    }

    hyperapi::Result rowsInTable =
        connection.executeQuery("SELECT * FROM " + extractTable.toString());
    for (const hyperapi::Row &row : rowsInTable) {
      std::size_t colNum = 0;
      for (const hyperapi::Value &value : row) {
        ARROW_RETURN_NOT_OK(append_funcs[colNum](value));
        colNum++;
      }
    }
  }

  std::vector<std::shared_ptr<arrow::Array>> arrays;
  for (std::size_t i = 0; i < colCount; i++) {
    std::shared_ptr<arrow::Array> array;
    auto maybe_array = builders[i]->Finish(&array);
    if (!maybe_array.ok()) {
      // TODO: handle failure
    }

    arrays.push_back(array);
  }

  auto table = arrow::Table::Make(schema, arrays);
  return table;
}
} // namespace hyperarrow
