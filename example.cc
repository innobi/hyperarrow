#include <iostream>

#include <hyperapi/hyperapi.hpp>
#include <arrow/builder.h>
#include <arrow/csv/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <arrow/pretty_print.h>
#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/table.h>

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
    ABORT_ON_FAILURE(builder.Finish(&array_a));
    builder.Reset();
    ABORT_ON_FAILURE(builder.AppendValues({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
    ABORT_ON_FAILURE(builder.Finish(&array_b));
    builder.Reset();
    ABORT_ON_FAILURE(builder.AppendValues({1, 2, 1, 2, 1, 2, 1, 2, 1, 2}));
    ABORT_ON_FAILURE(builder.Finish(&array_c));
    return arrow::Table::Make(schema, {array_a, array_b, array_c});
  }
    
  static const hyperapi::TableDefinition createDefinitionFromSchema(std::shared_ptr<arrow::Table> table) {
    const std::shared_ptr<arrow::Schema> schema = table->schema();
    hyperapi::TableDefinition tableDef = hyperapi::TableDefinition({"Extract", "Extract"});
    for (const arrow::Field field : schema->fields ) {
      // TODO: make a separate method to map arrow Fields to
      // Hyper column definitions
      hyperapi::TabbleDefinition::Column col = hyperapi::Column(field.name(), hyperapi::SqlType::bigInt(), field.nullable());
      tableDef.addColumn(col);
    }

    return tableDef;
  }
  
  static void insertTableIntoHyper(std::shared_ptr<arrow::Table> table) {
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

	    // TODO: replace with table values
	    inserter.addRow(1, 2, 3);
	    //for (auto i = 0; i < array->length(); i++) {
	    //  inserter.addRow(array->Value(i));
	      /*
	      if (array->IsValid(i)) {
		std::cerr << array->Value(i) << std::endl;      
	      } else {
		std::cerr << "Null Value!" << std::endl;
	      }
	      */
	    //}
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
