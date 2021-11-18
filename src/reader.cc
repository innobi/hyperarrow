#include <hyperarrow/reader.h>

#include <arrow/builder.h>
#include <arrow/table.h>

#include <hyperarpi/hyperapi.hpp>

#include <map>
#include "types.h"

static std::shared_ptr<arrow::Schema> arrowSchemaFromHyperTableDef(hyperapi::TableDefinition tableDefinition) {
  
}

static std::shared_ptr<arrow::Table> arrowTableFromHyperResult(hyperapi::Result result) {
  // See MakeColumnBuilders for reference in arrow::csv::reader.cc
  for (const hyperapi::Row& row : result) {
    for (const hyperapi::Value& value : row ) {
      // TODO: add to arrow table builders
    }
  }
}

namespace hyperarrow {
  std::shared_ptr<arrow::Table> arrowTableFromHyper(const std::string path) {
    hyperapi::HyperProcess hyper(hyperapi::Telemetry::DoNotSendUsageDataToTableau);
    {
      hyperapi::Connection connection(hyper.getEndpoint(), path);
      const hyperapi::Catalog& catalog = connection.getCatalog();
      hyperapi::TableName extractTable("Extract", "Extract");

      hyperapi::TableDefinition tableDefinition = catalog.getTableDefinition(extractTable);
      hyperapi::Result rowsInTable = connection.executeQuery("SELECT * FROM " + extractTable.ToString());
      
  }
}
