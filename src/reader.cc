#include <hyperarrow/reader.h>

#include <arrow/api.h>

#include <hyperapi/hyperapi.hpp>

#include <vector>

namespace hyperarrow {
  //static std::shared_ptr<arrow::Table> arrowTableFromHyperResult(hyperapi::Result result) {
  void printArrowTable() {
    // Create a schema describing datasets with two columns:
    std::shared_ptr<arrow::Field> field_a, field_b;
    std::shared_ptr<arrow::Schema> schema;

    field_a = arrow::field("A", arrow::int64());
    field_b = arrow::field("B", arrow::int64());
    schema = arrow::schema({field_a, field_b});

    std::size_t rowCount = 2;
    std::size_t colCount = 2;

    std::vector<arrow::Int64Builder> builders;
    for (std::size_t i = 0; i < colCount; i++) {
      arrow::Int64Builder tmp;
      tmp.Reserve(rowCount);
      builders.push_back(std::move(tmp));
    }

    std::size_t rowNum = 0;
    while ( rowNum < rowCount ) {
      std::size_t colNum = 0;
      while ( colNum < colCount ) {
	//for (auto rit = result.begin(); rit != result.end(); ++rit) {
	//  for (auto cit = *rit.begin(); cit != *rit.end(); ++cit) {
	builders[colNum].UnsafeAppend(1);
	colNum++;
      }
      rowNum++;
    }

    std::vector<std::shared_ptr<arrow::Array>> arrays;  
    for (std::size_t i = 0; i < colCount; i++) {
      std::shared_ptr<arrow::Array> array;      
      auto maybe_array = builders[i].Finish(&array);
      if (!maybe_array.ok()) {
	// TODO: handle failure
      }

      arrays.push_back(array);
    } 

    auto table = arrow::Table::Make(schema, arrays);
    arrow::PrettyPrint(*table, {}, &std::cerr);  
  }
  /*
  std::shared_ptr<arrow::Table> arrowTableFromHyper(const std::string path) {
    hyperapi::HyperProcess hyper(hyperapi::Telemetry::DoNotSendUsageDataToTableau);
    {
      hyperapi::Connection connection(hyper.getEndpoint(), path);
      const hyperapi::Catalog& catalog = connection.getCatalog();
      hyperapi::TableName extractTable("Extract", "Extract");

      hyperapi::TableDefinition tableDefinition = catalog.getTableDefinition(extractTable);
      hyperapi::Result rowsInTable = connection.executeQuery("SELECT * FROM " + extractTable.ToString());
      
  }
  */
}
