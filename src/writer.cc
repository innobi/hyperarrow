#include <hyperarrow/writer.h>

#include <arrow/builder.h>
#include <arrow/compute/api.h>
#include <arrow/table.h>

#include <hyperapi/hyperapi.hpp>
#include <map>

#include "types.h"

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

///
/// Maps arrow::DateTimeArrays to a map of maps. The top level map
/// uses the 0-based index of the array in the provided table as its key
/// The inner map uses the constant "year", "month", "day" as values
///
/// Ideally we could just call a compute function that maps multiple functions
/// to a struct containing year / month / day, just wasn't sure how to do that
/// :-)
static const std::map<
    int, std::map<const std::string, std::shared_ptr<arrow::Int64Array>>>
mapDateArraysToComponents(const std::shared_ptr<arrow::Table> table) {
  std::map<int, std::map<const std::string, std::shared_ptr<arrow::Int64Array>>>
      result;
  const std::shared_ptr<arrow::Schema> schema = table->schema();
  for (int i = 0; i < schema->num_fields(); i++) {
    if (schema->field(i)->type() == arrow::date32()) {
      auto array = std::static_pointer_cast<arrow::Date32Array>(
          table->column(i)->chunk(0));

      // ARROW_ASSIGN_OR_RAISE(arrow::Datum year_datum,
      // arrow::compute::Year(array)); ARROW_ASSIGN_OR_RAISE macro wasn't
      // working. Yielded error: error: could not convert '(&
      // _error_or_value7)->arrow::Result<arrow::Datum>
      // ::status()' from 'const arrow::Status' to 'const std::map<int,
      // std::map< const std::__cxx11::basic_string<char>,
      // std::shared_ptr<arrow::NumericArray< arrow::Int64Type> > > >
      //
      // Possibly a bug with the macro?
      arrow::Datum year_datum;
      auto year_datum_result = arrow::compute::CallFunction("year", {array});
      if (!year_datum_result.ok()) {
        // TODO: handle error
      } else {
        year_datum = year_datum_result.ValueOrDie();
      }
      arrow::Datum month_datum;
      auto month_datum_result = arrow::compute::CallFunction("month", {array});
      if (!month_datum_result.ok()) {
        // TODO: handle error
      } else {
        month_datum = month_datum_result.ValueOrDie();
      }
      arrow::Datum day_datum;
      auto day_datum_result = arrow::compute::CallFunction("day", {array});
      if (!day_datum_result.ok()) {
        // TODO: handle error
      } else {
        day_datum = day_datum_result.ValueOrDie();
      }

      std::shared_ptr<arrow::Array> years_arr = year_datum.make_array();
      std::shared_ptr<arrow::Array> months_arr = month_datum.make_array();
      std::shared_ptr<arrow::Array> days_arr = day_datum.make_array();

      std::shared_ptr<arrow::Int64Array> years =
          std::dynamic_pointer_cast<arrow::Int64Array>(years_arr);
      std::shared_ptr<arrow::Int64Array> months =
          std::dynamic_pointer_cast<arrow::Int64Array>(months_arr);
      std::shared_ptr<arrow::Int64Array> days =
          std::dynamic_pointer_cast<arrow::Int64Array>(days_arr);

      std::map<const std::string, std::shared_ptr<arrow::Int64Array>>
          innerResult;
      innerResult["year"] = years;
      innerResult["month"] = months;
      innerResult["day"] = days;
      result.insert({i, innerResult});
    }
  }

  return result;
}

///
/// Maps arrow::TimestampArrays to a map of maps. The top level map
/// uses the 0-based index of the array in the provided table as its key
/// The inner map uses the constant "year", "month", "day" as values
///
/// Ideally we could just call a compute function that maps multiple functions
/// to a struct containing year / month / day, just wasn't sure how to do that
/// :-)
static const std::map<
    int, std::map<const std::string, std::shared_ptr<arrow::Int64Array>>>
mapTsArraysToComponents(const std::shared_ptr<arrow::Table> table) {
  std::map<int, std::map<const std::string, std::shared_ptr<arrow::Int64Array>>>
      result;
  const std::shared_ptr<arrow::Schema> schema = table->schema();
  for (int i = 0; i < schema->num_fields(); i++) {
    if (schema->field(i)->type()->id() == arrow::Type::TIMESTAMP) {
      auto array = std::static_pointer_cast<arrow::TimestampArray>(
          table->column(i)->chunk(0));

      // ARROW_ASSIGN_OR_RAISE(arrow::Datum year_datum,
      // arrow::compute::Year(array)); ARROW_ASSIGN_OR_RAISE macro wasn't
      // working. Yielded error: error: could not convert '(&
      // _error_or_value7)->arrow::Result<arrow::Datum>
      // ::status()' from 'const arrow::Status' to 'const std::map<int,
      // std::map< const std::__cxx11::basic_string<char>,
      // std::shared_ptr<arrow::NumericArray< arrow::Int64Type> > > >
      //
      // Possibly a bug with the macro?
      arrow::Datum year_datum;
      auto year_datum_result = arrow::compute::CallFunction("year", {array});
      if (!year_datum_result.ok()) {
        // TODO: handle error
      } else {
        year_datum = year_datum_result.ValueOrDie();
      }
      arrow::Datum month_datum;
      auto month_datum_result = arrow::compute::CallFunction("month", {array});
      if (!month_datum_result.ok()) {
        // TODO: handle error
      } else {
        month_datum = month_datum_result.ValueOrDie();
      }
      arrow::Datum day_datum;
      auto day_datum_result = arrow::compute::CallFunction("day", {array});
      if (!day_datum_result.ok()) {
        // TODO: handle error
      } else {
        day_datum = day_datum_result.ValueOrDie();
      }

      arrow::Datum hour_datum;
      auto hour_datum_result = arrow::compute::CallFunction("hour", {array});
      if (!hour_datum_result.ok()) {
        // TODO: handle error
      } else {
        hour_datum = hour_datum_result.ValueOrDie();
      }

      arrow::Datum minute_datum;
      auto minute_datum_result =
          arrow::compute::CallFunction("minute", {array});
      if (!minute_datum_result.ok()) {
        // TODO: handle error
      } else {
        minute_datum = minute_datum_result.ValueOrDie();
      }

      arrow::Datum second_datum;
      auto second_datum_result =
          arrow::compute::CallFunction("second", {array});
      if (!second_datum_result.ok()) {
        // TODO: handle error
      } else {
        second_datum = second_datum_result.ValueOrDie();
      }

      arrow::Datum microsecond_datum;
      auto microsecond_datum_result =
          arrow::compute::CallFunction("microsecond", {array});
      if (!microsecond_datum_result.ok()) {
        // TODO: handle error
      } else {
        microsecond_datum = microsecond_datum_result.ValueOrDie();
      }

      std::shared_ptr<arrow::Array> years_arr = year_datum.make_array();
      std::shared_ptr<arrow::Array> months_arr = month_datum.make_array();
      std::shared_ptr<arrow::Array> days_arr = day_datum.make_array();
      std::shared_ptr<arrow::Array> hours_arr = hour_datum.make_array();
      std::shared_ptr<arrow::Array> minutes_arr = minute_datum.make_array();
      std::shared_ptr<arrow::Array> seconds_arr = second_datum.make_array();
      std::shared_ptr<arrow::Array> microseconds_arr =
          microsecond_datum.make_array();

      std::shared_ptr<arrow::Int64Array> years =
          std::dynamic_pointer_cast<arrow::Int64Array>(years_arr);
      std::shared_ptr<arrow::Int64Array> months =
          std::dynamic_pointer_cast<arrow::Int64Array>(months_arr);
      std::shared_ptr<arrow::Int64Array> days =
          std::dynamic_pointer_cast<arrow::Int64Array>(days_arr);
      std::shared_ptr<arrow::Int64Array> hours =
          std::dynamic_pointer_cast<arrow::Int64Array>(hours_arr);
      std::shared_ptr<arrow::Int64Array> minutes =
          std::dynamic_pointer_cast<arrow::Int64Array>(minutes_arr);
      std::shared_ptr<arrow::Int64Array> seconds =
          std::dynamic_pointer_cast<arrow::Int64Array>(seconds_arr);
      std::shared_ptr<arrow::Int64Array> microseconds =
          std::dynamic_pointer_cast<arrow::Int64Array>(microseconds_arr);

      std::map<const std::string, std::shared_ptr<arrow::Int64Array>>
          innerResult;
      innerResult["year"] = years;
      innerResult["month"] = months;
      innerResult["day"] = days;
      innerResult["hour"] = hours;
      innerResult["minute"] = minutes;
      innerResult["second"] = seconds;
      innerResult["microsecond"] = microseconds;
      result.insert({i, innerResult});
    }
  }

  return result;
}

void arrowTableToHyper(const std::shared_ptr<arrow::Table> table,
                       const std::string databasePath,
                       const std::string schemaName,
                       const std::string tableName) {
  {
    auto dateComponents = mapDateArraysToComponents(table);
    auto tsComponents = mapTsArraysToComponents(table);

    std::vector<std::function<void(std::shared_ptr<arrow::Array> anArray,
                                   hyperapi::Inserter & inserter,
                                   int64_t colNum, int64_t rowNum)>>
        write_funcs;
    for (auto &field : table->fields()) {
      auto type = field->type();
      if (type == arrow::int16()) {
        write_funcs.push_back([dateComponents, tsComponents](
                                  std::shared_ptr<arrow::Array> anArray,
                                  hyperapi::Inserter &inserter, int64_t colNum,
                                  int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::Int16Array>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->Value(rowNum));
          } else {
            inserter.add(hyperapi::optional<int16_t>());
          }
        });
      } else if (type == arrow::int32()) {
        write_funcs.push_back([dateComponents, tsComponents](
                                  std::shared_ptr<arrow::Array> anArray,
                                  hyperapi::Inserter &inserter, int64_t colNum,
                                  int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::Int32Array>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->Value(rowNum));
          } else {
            inserter.add(hyperapi::optional<int32_t>());
          }
        });
      } else if (type == arrow::int64()) {
        write_funcs.push_back([dateComponents, tsComponents](
                                  std::shared_ptr<arrow::Array> anArray,
                                  hyperapi::Inserter &inserter, int64_t colNum,
                                  int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::Int64Array>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->Value(rowNum));
          } else {
            inserter.add(hyperapi::optional<int64_t>());
          }
        });
      } else if (type == arrow::float32()) {
        write_funcs.push_back([dateComponents, tsComponents](
                                  std::shared_ptr<arrow::Array> anArray,
                                  hyperapi::Inserter &inserter, int64_t colNum,
                                  int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::FloatArray>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->Value(rowNum));
          } else {
            inserter.add(hyperapi::optional<double_t>());
          }
        });
      } else if (type == arrow::float64()) {
        write_funcs.push_back([dateComponents, tsComponents](
                                  std::shared_ptr<arrow::Array> anArray,
                                  hyperapi::Inserter &inserter, int64_t colNum,
                                  int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::DoubleArray>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->Value(rowNum));
          } else {
            inserter.add(hyperapi::optional<double_t>());
          }
        });
      } else if (type == arrow::boolean()) {
        write_funcs.push_back([dateComponents, tsComponents](
                                  std::shared_ptr<arrow::Array> anArray,
                                  hyperapi::Inserter &inserter, int64_t colNum,
                                  int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::BooleanArray>(anArray);
          if (array->IsValid(rowNum)) {
            inserter.add(array->Value(rowNum));
          } else {
            inserter.add(hyperapi::optional<bool>());
          }
        });
      } else if (type == arrow::date32()) {
        write_funcs.push_back([dateComponents, tsComponents](
                                  std::shared_ptr<arrow::Array> anArray,
                                  hyperapi::Inserter &inserter, int64_t colNum,
                                  int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::Date32Array>(anArray);
          if (array->IsValid(rowNum)) {
            int64_t year, month, day;
            auto search = dateComponents.find(colNum);
            if (search == dateComponents.end()) {
              // TODO:: some error
            } else {
              auto dateMap = search->second;
              auto yearSearch = dateMap.find("year");
              if (yearSearch == dateMap.end()) {
                // TODO:: handle error
              } else {
                year = yearSearch->second->Value(rowNum);
              }
              auto monthSearch = dateMap.find("month");
              if (monthSearch == dateMap.end()) {
                // TODO:: handle error
              } else {
                month = monthSearch->second->Value(rowNum);
              }
              auto daySearch = dateMap.find("day");
              if (daySearch == dateMap.end()) {
                // TODO:: handle error
              } else {
                day = daySearch->second->Value(rowNum);
              }
            }
            inserter.add(hyperapi::Date(year, month, day));
          } else {
            inserter.add(hyperapi::optional<hyperapi::Date>());
          }
        });
      } else if (type->id() == arrow::Type::TIMESTAMP) {
        write_funcs.push_back([dateComponents, tsComponents](
                                  std::shared_ptr<arrow::Array> anArray,
                                  hyperapi::Inserter &inserter, int64_t colNum,
                                  int64_t rowNum) {
          auto array = std::static_pointer_cast<arrow::TimestampArray>(anArray);
          if (array->IsValid(rowNum)) {
            int64_t year, month, day, hour, minute, second, microsecond;
            auto search = tsComponents.find(colNum);
            if (search == tsComponents.end()) {
              // TODO: handle error
            } else {
              auto tsMap = search->second;
              auto yearSearch = tsMap.find("year");
              if (yearSearch == tsMap.end()) {
                // TODO:: handle error
              } else {
                year = yearSearch->second->Value(rowNum);
              }
              auto monthSearch = tsMap.find("month");
              if (monthSearch == tsMap.end()) {
                // TODO:: handle error
              } else {
                month = monthSearch->second->Value(rowNum);
              }
              auto daySearch = tsMap.find("day");
              if (daySearch == tsMap.end()) {
                // TODO:: handle error
              } else {
                day = daySearch->second->Value(rowNum);
              }
              auto hourSearch = tsMap.find("hour");
              if (hourSearch == tsMap.end()) {
                // TODO:: handle error
              } else {
                hour = hourSearch->second->Value(rowNum);
              }
              auto minuteSearch = tsMap.find("minute");
              if (minuteSearch == tsMap.end()) {
                // TODO:: handle error
              } else {
                minute = minuteSearch->second->Value(rowNum);
              }
              auto secondSearch = tsMap.find("second");
              if (secondSearch == tsMap.end()) {
                // TODO:: handle error
              } else {
                second = secondSearch->second->Value(rowNum);
              }
              auto microsecondSearch = tsMap.find("microsecond");
              if (microsecondSearch == tsMap.end()) {
                // TODO:: handle error
              } else {
                microsecond = microsecondSearch->second->Value(rowNum);
              }
            }
            auto time = hyperapi::Time(hour, minute, second, microsecond);
            auto date = hyperapi::Date(year, month, day);
            inserter.add(hyperapi::Timestamp(date, time));
          } else {
            inserter.add(hyperapi::optional<hyperapi::Timestamp>());
          }
        });
      } else if (type == arrow::utf8()) {
        write_funcs.push_back([dateComponents, tsComponents](
                                  std::shared_ptr<arrow::Array> anArray,
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
      static const hyperapi::TableDefinition extractTable =
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
