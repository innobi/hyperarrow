#include "writer.h"

#include <arrow/builder.h>
#include <arrow/compute/api.h>
#include <arrow/table.h>

#include <hyperapi/hyperapi.hpp>
#include <map>

#include "types.h"

namespace hyperarrow {
static const hyperapi::TableDefinition createDefinitionFromSchema(
    std::shared_ptr<arrow::Table> table) {
  const std::shared_ptr<arrow::Schema> schema = table->schema();
  hyperapi::TableDefinition tableDef =
      hyperapi::TableDefinition({"Extract", "Extract"});
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
      auto year_datum_result = arrow::compute::Year(array);
      if (!year_datum_result.ok()) {
        // TODO: handle error
      } else {
        year_datum = year_datum_result.ValueOrDie();
      }
      arrow::Datum month_datum;
      auto month_datum_result = arrow::compute::Month(array);
      if (!month_datum_result.ok()) {
        // TODO: handle error
      } else {
        month_datum = month_datum_result.ValueOrDie();
      }
      arrow::Datum day_datum;
      auto day_datum_result = arrow::compute::Day(array);
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

void arrowTableToHyper(const std::shared_ptr<arrow::Table> table,
                       const std::string path) {
  const std::string pathToDatabase = "example.hyper";
  {
    auto dateComponents = mapDateArraysToComponents(table);
    hyperapi::HyperProcess hyper(
        hyperapi::Telemetry::DoNotSendUsageDataToTableau);
    {
      hyperapi::Connection connection(hyper.getEndpoint(), pathToDatabase,
                                      hyperapi::CreateMode::CreateAndReplace);
      const hyperapi::Catalog& catalog = connection.getCatalog();
      static const hyperapi::TableDefinition extractTable =
          createDefinitionFromSchema(table);

      catalog.createSchema("Extract");
      catalog.createTable(extractTable);
      {
        hyperapi::Inserter inserter{connection, extractTable};
        for (int64_t i = 0; i < table->num_rows(); i++) {
          for (int64_t j = 0; j < table->num_columns(); j++) {
            // TODO: templating could likely help a ton here
            const std::shared_ptr<arrow::DataType> type =
                table->field(j)->type();
            if (type == arrow::int16()) {
              auto array = std::static_pointer_cast<arrow::Int16Array>(
                  table->column(j)->chunk(0));
              if (array->IsValid(i)) {
                inserter.add(array->Value(i));
              } else {
                inserter.add(hyperapi::optional<int16_t>());
              }
            } else if (type == arrow::int32()) {
              auto array = std::static_pointer_cast<arrow::Int32Array>(
                  table->column(j)->chunk(0));
              if (array->IsValid(i)) {
                inserter.add(array->Value(i));
              } else {
                inserter.add(hyperapi::optional<int32_t>());
              }
            } else if (type == arrow::int64()) {
              auto array = std::static_pointer_cast<arrow::Int64Array>(
                  table->column(j)->chunk(0));
              if (array->IsValid(i)) {
                inserter.add(array->Value(i));
              } else {
                inserter.add(hyperapi::optional<int64_t>());
              }
            } else if (type == arrow::float16()) {
              auto array = std::static_pointer_cast<arrow::HalfFloatArray>(
                  table->column(j)->chunk(0));
              if (array->IsValid(i)) {
                inserter.add(array->Value(i));
              } else {
                inserter.add(hyperapi::optional<double_t>());
              }
            } else if (type == arrow::float32()) {
              auto array = std::static_pointer_cast<arrow::FloatArray>(
                  table->column(j)->chunk(0));
              if (array->IsValid(i)) {
                inserter.add(array->Value(i));
              } else {
                inserter.add(hyperapi::optional<double_t>());
              }
            } else if (type == arrow::float64()) {
              auto array = std::static_pointer_cast<arrow::DoubleArray>(
                  table->column(j)->chunk(0));
              if (array->IsValid(i)) {
                inserter.add(array->Value(i));
              } else {
                inserter.add(hyperapi::optional<double_t>());
              }
            } else if (type == arrow::boolean()) {
              auto array = std::static_pointer_cast<arrow::BooleanArray>(
                  table->column(j)->chunk(0));
              if (array->IsValid(i)) {
                inserter.add(array->Value(i));
              } else {
                inserter.add(hyperapi::optional<bool>());
              }
            } else if (type == arrow::date32()) {
              auto array = std::static_pointer_cast<arrow::Date32Array>(
                  table->column(j)->chunk(0));
              if (array->IsValid(i)) {
                int64_t year, month, day;
                auto search = dateComponents.find(j);
                if (search == dateComponents.end()) {
                  // TODO:: some error
                } else {
                  auto dateMap = search->second;
                  auto yearSearch = dateMap.find("year");
                  if (yearSearch == dateMap.end()) {
                    // TODO:: handle error
                  } else {
                    year = yearSearch->second->Value(i);
                  }
                  auto monthSearch = dateMap.find("month");
                  if (monthSearch == dateMap.end()) {
                    // TODO:: handle error
                  } else {
                    month = monthSearch->second->Value(i);
                  }
                  auto daySearch = dateMap.find("day");
                  if (daySearch == dateMap.end()) {
                    // TODO:: handle error
                  } else {
                    day = daySearch->second->Value(i);
                  }
                }
                inserter.add(hyperapi::Date(year, month, day));
              } else {
                inserter.add(hyperapi::optional<hyperapi::Date>());
              }
            }
          }
          inserter.endRow();
        }
        inserter.execute();
      }
    }
  }
}
}  // namespace hyperarrow
