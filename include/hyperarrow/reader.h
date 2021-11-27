#include <arrow/table.h>

namespace hyperarrow {
  arrow::Result<std::shared_ptr<arrow::Table>> arrowTableFromHyper(
                       const std::string databasePath,
		       const std::string schemaName,
		       const std::string tableName);
}
