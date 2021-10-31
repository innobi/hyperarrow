#include "writer.h"
#include "types.h"

#include <hyperapi/hyperapi.hpp>
#include <arrow/builder.h>
#include <arrow/table.h>

namespace hyperarrow {
  static const hyperapi::TableDefinition createDefinitionFromSchema(std::shared_ptr<arrow::Table> table) {
    const std::shared_ptr<arrow::Schema> schema = table->schema();
    hyperapi::TableDefinition tableDef = hyperapi::TableDefinition({"Extract", "Extract"});
    for (const std::shared_ptr<arrow::Field> field : schema->fields() ) {
      // TODO: without these conversions can easily get an error like
      // error: no matching function for call to 'hyperapi::TableDefinition::Column::Column(const string&, hyperapi::SqlType, bool)'
      hyperapi::Name name = hyperapi::Name{field->name()};
      hyperapi::Nullability nullable = field->nullable() ? hyperapi::Nullability::Nullable : hyperapi::Nullability::NotNullable;
      hyperapi::SqlType type = hyperarrow::arrowTypeToSqlType(field->type());
      
      hyperapi::TableDefinition::Column col = hyperapi::TableDefinition::Column(name, type, nullable);
      tableDef.addColumn(col);
    }

    return tableDef;
  }

  void arrowTableToHyper(const std::shared_ptr<arrow::Table> table, const std::string path) {
    const std::string pathToDatabase = "example.hyper";
      {
	hyperapi::HyperProcess hyper(hyperapi::Telemetry::DoNotSendUsageDataToTableau);
	{
	  hyperapi::Connection connection(hyper.getEndpoint(), pathToDatabase, hyperapi::CreateMode::CreateAndReplace);
	  const hyperapi::Catalog& catalog = connection.getCatalog();
	  static const hyperapi::TableDefinition extractTable = createDefinitionFromSchema(table);

	  catalog.createSchema("Extract");
	  catalog.createTable(extractTable);
	  {
	    hyperapi::Inserter inserter{connection, extractTable};
	    for (int64_t i = 0; i < table->num_rows(); i++) {
	      for (int64_t j = 0; j < table->num_columns(); j++) {
		// TODO: templating could likely help a ton here
		const std::shared_ptr<arrow::DataType> type = table->field(j)->type();
		if (type == arrow::int16()) {
		  auto array = std::static_pointer_cast<arrow::Int16Array>(table->column(j)->chunk(0));
		  if (array->IsValid(i)) {
		    inserter.add(array->Value(i));		  
		  } else {
		    inserter.add(hyperapi::optional<int16_t>());
		  }
		} else if (type == arrow::int32()) {
		  auto array = std::static_pointer_cast<arrow::Int32Array>(table->column(j)->chunk(0));
		  if (array->IsValid(i)) {
		    inserter.add(array->Value(i));		  
		  } else {
		    inserter.add(hyperapi::optional<int32_t>());
		  }
		}
		else if (type == arrow::int64()) {
		  auto array = std::static_pointer_cast<arrow::Int64Array>(table->column(j)->chunk(0));
		  if (array->IsValid(i)) {
		    inserter.add(array->Value(i));		  
		  } else {
		    inserter.add(hyperapi::optional<int64_t>());
		  }
		} else if (type == arrow::float16()) {
		  auto array = std::static_pointer_cast<arrow::HalfFloatArray>(table->column(j)->chunk(0));
		  if (array->IsValid(i)) {
		    inserter.add(array->Value(i));		  
		  } else {
		    inserter.add(hyperapi::optional<double_t>());
		  }
		} else if (type == arrow::float32()) {
		  auto array = std::static_pointer_cast<arrow::FloatArray>(table->column(j)->chunk(0));
		  if (array->IsValid(i)) {
		    inserter.add(array->Value(i));		  
		  } else {
		    inserter.add(hyperapi::optional<double_t>());
		  }
		} else if (type == arrow::float64()) {
		  auto array = std::static_pointer_cast<arrow::DoubleArray>(table->column(j)->chunk(0));
		  if (array->IsValid(i)) {
		    inserter.add(array->Value(i));		  
		  } else {
		    inserter.add(hyperapi::optional<double_t>());
		  }
		} else if (type == arrow::boolean()) {
		  auto array = std::static_pointer_cast<arrow::BooleanArray>(table->column(j)->chunk(0));
		  if (array->IsValid(i)) {
		    inserter.add(array->Value(i));		  
		  } else {
		    inserter.add(hyperapi::optional<bool>());
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
}
