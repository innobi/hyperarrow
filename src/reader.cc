#include <arrow/api.h>
#include <hyperapi/hyperapi.hpp>
#include <vector>
#include "hyperarrow/reader.h"
#include "types.h"

static std::shared_ptr<arrow::Schema> schemaFromHyper(hyperapi::TableDefinition tableDefinition) {
  std::vector<std::shared_ptr<arrow::Field>> fields;
  for (auto& column : tableDefinition.getColumns()) {
    auto type = hyperarrow::hyperTypeToArrowType(column.getType());
    auto field = arrow::field(column.getName().toString(), type);
    fields.push_back(field);
  }
  
  std::shared_ptr<arrow::Schema> schema;

  schema = arrow::schema(fields);

  return schema;
}

namespace hyperarrow {
  //static std::shared_ptr<arrow::Table> arrowTableFromHyperResult(hyperapi::Result result) {
  void printArrowTable() {
    auto path = "read_example.hyper";
    std::size_t colCount;
    std::vector<std::shared_ptr<arrow::ArrayBuilder>> builders;
    std::shared_ptr<arrow::Schema> schema;
    
    hyperapi::HyperProcess hyper(hyperapi::Telemetry::DoNotSendUsageDataToTableau);
    {
      hyperapi::Connection connection(hyper.getEndpoint(), path);
      const hyperapi::Catalog& catalog = connection.getCatalog();
      hyperapi::TableName extractTable("Extract", "Extract");

      hyperapi::TableDefinition tableDefinition = catalog.getTableDefinition(extractTable);
      schema = schemaFromHyper(tableDefinition);
      colCount = schema->num_fields();
      //std::size_t rowCount = 2;

      std::vector<std::function<arrow::Status(const hyperapi::Value& value)>> append_funcs;
      for (int i = 0; i < schema->fields().size(); i++) {
	const auto& field = schema->fields()[i];
	if (schema->field(i)->type() == arrow::int16()) {
	  auto builder = std::make_shared<arrow::Int16Builder>();
	  append_funcs.push_back([builder] (const hyperapi::Value& value) {
	    return builder->Append(value);
	  });
	  // TODO: if we get rowCount up front we can more efficiently append
	  //builder->Reserve(rowCount);	
	  builders.push_back(std::move(builder));
	}
	else if (schema->field(i)->type() == arrow::int32()) {
	  auto builder = std::make_shared<arrow::Int32Builder>();
	  append_funcs.push_back([builder] (const hyperapi::Value& value) {
	    return builder->Append(value);
	  });
	  
	  // TODO: if we get rowCount up front we can more efficiently append
	  //builder->Reserve(rowCount);	
	  builders.push_back(std::move(builder));
	}
	else if (schema->field(i)->type() == arrow::int64()) {
	  auto builder = std::make_shared<arrow::Int64Builder>();
	  append_funcs.push_back([builder] (const hyperapi::Value& value) {
	    // TODO; this sentinel should be replaced by a value
	    // coming from the Hyper extract
	    return builder->Append(value);
	  });
	  
	  // TODO: if we get rowCount up front we can more efficiently append
	  //builder->Reserve(rowCount);	
	  builders.push_back(std::move(builder));
	}
	else if (schema->field(i)->type() == arrow::float64()) {
	  auto builder = std::make_shared<arrow::DoubleBuilder>();
	  append_funcs.push_back([builder] (const hyperapi::Value& value) {
	    return builder->Append(value);
	  });
	  
	  // TODO: if we get rowCount up front we can more efficiently append
	  //int_builder->Reserve(rowCount);	
	  builders.push_back(std::move(builder));
	}
	else if (schema->field(i)->type() == arrow::boolean()) {
	  auto builder = std::make_shared<arrow::BooleanBuilder>();
	  append_funcs.push_back([builder] (const hyperapi::Value& value) {
	    return builder->Append(static_cast<bool>(value));
	  });
	  
	  // TODO: if we get rowCount up front we can more efficiently append
	  //int_builder->Reserve(rowCount);	
	  builders.push_back(std::move(builder));
	}
      }

      
      hyperapi::Result rowsInTable = connection.executeQuery("SELECT * FROM " + extractTable.toString());
      for (const hyperapi::Row& row : rowsInTable) {
	std::size_t colNum = 0;
	for (const hyperapi::Value& value : row) {
	  append_funcs[colNum](value);
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
    arrow::PrettyPrint(*table, {}, &std::cerr);  
  }
}
