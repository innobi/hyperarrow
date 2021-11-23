#include <hyperarrow/reader.h>

#include <arrow/api.h>

#include <hyperapi/hyperapi.hpp>

#include <vector>

static std::shared_ptr<arrow::Schema> schemaFromHyper() {
    std::shared_ptr<arrow::Field> field_a, field_b;
    std::shared_ptr<arrow::Schema> schema;

    field_a = arrow::field("A", arrow::int64());
    field_b = arrow::field("B", arrow::int64());
    schema = arrow::schema({field_a, field_b});

    return schema;
}

namespace hyperarrow {
  //static std::shared_ptr<arrow::Table> arrowTableFromHyperResult(hyperapi::Result result) {
  void printArrowTable() {
    auto schema = schemaFromHyper();
    auto colCount = schema->num_fields();
    std::size_t rowCount = 2;

    std::vector<std::function<arrow::Status()>> append_funcs;
    std::vector<std::shared_ptr<arrow::ArrayBuilder>> builders;
    for (int i = 0; i < schema->fields().size(); i++) {
      const auto& field = schema->fields()[i];
      if (schema->field(i)->type() == arrow::int64()) {
	auto int_builder = std::make_shared<arrow::Int64Builder>();
	append_funcs.push_back([int_builder] () {
	      // TODO; this sentinel should be replaced by a value
	      // coming from the Hyper extract
	      int val = 1;
	      return int_builder->Append(val);
	});
	int_builder->Reserve(rowCount);	
	builders.push_back(std::move(int_builder));
      }
    }

    std::size_t rowNum = 0;
    while ( rowNum < rowCount ) {
      std::size_t colNum = 0;
      while ( colNum < colCount ) {
	//for (auto rit = result.begin(); rit != result.end(); ++rit) {
	//  for (auto cit = *rit.begin(); cit != *rit.end(); ++cit) {
	//builders[colNum].UnsafeAppend(1);
	//ARROW_RETURN_NOT_OK(
	append_funcs[colNum]();//);
	colNum++;
      }
      rowNum++;
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
