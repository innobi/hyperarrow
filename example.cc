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

namespace {

  std::shared_ptr<arrow::Int64Array> BuildData() {
    arrow::Int64Builder builder;
    builder.Resize(8);
    std::vector<bool> validity = {true, true, true, false, true, true, true, true};
    std::vector<int64_t> values = {1, 2, 3, 0, 5, 6, 7, 8};
    builder.AppendValues(values, validity);

    std::shared_ptr<arrow::Int64Array> array;
    arrow::Status st = builder.Finish(&array);
    if (!st.ok()) {
      // handle error here
    }

    return array;
  }
  
  static void insertArrayIntoHyperTable(std::shared_ptr<arrow::Int64Array> array) {
    const std::string pathToDatabase = "example.hyper";
      {
	hyperapi::HyperProcess hyper(hyperapi::Telemetry::DoNotSendUsageDataToTableau);
	{
	  hyperapi::Connection connection(hyper.getEndpoint(), pathToDatabase, hyperapi::CreateMode::CreateAndReplace);
	  const hyperapi::Catalog& catalog = connection.getCatalog();

	  static const hyperapi::TableDefinition extractTable{
	    {"Extract", "Extract"},
	    {hyperapi::TableDefinition::Column{"A Number", hyperapi::SqlType::bigInt(), hyperapi::Nullability::NotNullable}}
	  };

	  catalog.createSchema("Extract");
	  catalog.createTable(extractTable);
	  {
	    hyperapi::Inserter inserter(connection, extractTable);
	    
	    for (auto i = 0; i < array->length(); i++) {
	      inserter.addRow(array->Value(i));
	      /*
	      if (array->IsValid(i)) {
		std::cerr << array->Value(i) << std::endl;      
	      } else {
		std::cerr << "Null Value!" << std::endl;
	      }
	      */
	    }
	    inserter.execute();	    
	  }

         std::unordered_set<hyperapi::TableName> tableNames = catalog.getTableNames("Extract");
         std::cout << "Tables available in " << pathToDatabase << " in the Extract schema are: ";
         for (auto& tableName : tableNames)
            std::cout << tableName.toString() << "\t";
         std::cout << std::endl;

         // Number of rows in the "Extract"."Extract" table.
         // `executeScalarQuery` is for executing a query that returns exactly one row with one column.
         int64_t rowCount = connection.executeScalarQuery<int64_t>("SELECT COUNT(*) FROM " + extractTable.getTableName().toString());
         std::cout << "The number of rows in table " << extractTable.getTableName() << " is " << rowCount << "." << std::endl;	  
	}
      }
  }

Status RunMain(int argc, char** argv) {
  const char* csv_filename = "test.csv";
  const char* arrow_filename = "test.arrow";

  std::cerr << "* Reading CSV file '" << csv_filename << "' into table" << std::endl;
  ARROW_ASSIGN_OR_RAISE(auto input_file,
                        arrow::io::ReadableFile::Open(csv_filename));
  ARROW_ASSIGN_OR_RAISE(
      auto csv_reader,
      arrow::csv::TableReader::Make(arrow::io::default_io_context(),
                                    input_file,
                                    arrow::csv::ReadOptions::Defaults(),
                                    arrow::csv::ParseOptions::Defaults(),
                                    arrow::csv::ConvertOptions::Defaults()));
  ARROW_ASSIGN_OR_RAISE(auto table, csv_reader->Read());

  std::cerr << "* Generating data:" << std::endl;
  std::shared_ptr<arrow::Int64Array> array = BuildData();

  std::cerr << "* Creating Hyper File:" << std::endl;
  insertArrayIntoHyperTable(array);
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
