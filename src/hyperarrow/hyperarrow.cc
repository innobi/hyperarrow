#include <iostream>

#include <hyperapi/hyperapi.hpp>
#include <arrow/builder.h>
#include <arrow/table.h>

#include "types.h"

using arrow::Status;

#define ABORT_ON_FAILURE(expr)                     \
  do {                                             \
    arrow::Status status_ = (expr);                \
    if (!status_.ok()) {                           \
      std::cerr << status_.message() << std::endl; \
      abort();                                     \
    }                                              \
  } while (0);


namespace {

  std::shared_ptr<arrow::Table> createTable() {
    auto schema =
      arrow::schema({
	  arrow::field("a", arrow::int64()),
	  arrow::field("b", arrow::int64()),
	  arrow::field("c", arrow::int64())
	});
    std::shared_ptr<arrow::Array> array_a;
    std::shared_ptr<arrow::Array> array_b;
    std::shared_ptr<arrow::Array> array_c;
    arrow::NumericBuilder<arrow::Int64Type> builder;
    ABORT_ON_FAILURE(builder.AppendValues({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    ABORT_ON_FAILURE(builder.AppendNull());
    ABORT_ON_FAILURE(builder.Finish(&array_a));
    builder.Reset();
    ABORT_ON_FAILURE(builder.AppendValues({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
    ABORT_ON_FAILURE(builder.AppendNull());
    ABORT_ON_FAILURE(builder.Finish(&array_b));
    builder.Reset();
    ABORT_ON_FAILURE(builder.AppendValues({1, 2, 1, 2, 1, 2, 1, 2, 1, 2}));
    ABORT_ON_FAILURE(builder.AppendNull());
    ABORT_ON_FAILURE(builder.Finish(&array_c));
    return arrow::Table::Make(schema, {array_a, array_b, array_c});
  }
    
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

  static void insertTableIntoHyper(const std::shared_ptr<arrow::Table> table) {
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
		auto array = std::static_pointer_cast<arrow::Int64Array>(table->column(j)->chunk(0));
		if (array->IsValid(i)) {
		  inserter.add(array->Value(i));		  
		} else {
		  inserter.add(hyperapi::optional<int64_t>());
		}
	      }
	      inserter.endRow();
	    }
	    inserter.execute();	    
	  }
	}
      }
  }

Status RunMain(int argc, char** argv) {
  std::cerr << "* Generating data:" << std::endl;
  std::shared_ptr<arrow::Table> table = createTable();  

  std::cerr << "* Creating Hyper File:" << std::endl;
  insertTableIntoHyper(table);
  std::cerr << "* Hyper File Created Successfullly!" << std::endl;

  return Status::OK();
}

}  // namespace

int main(int argc, char** argv) {
  Status st = RunMain(argc, argv);
  if (!st.ok()) {
    std::cerr << st << std::endl;
    return 1;
  }
  return 0;
}
