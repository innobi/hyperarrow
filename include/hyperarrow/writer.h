#include <arrow/table.h>

namespace hyperarrow {
void arrowTableToHyper(const std::shared_ptr<arrow::Table> table,
                       const std::string databasePath,
		       const std::string schemaName,
		       const std::string tableName
		       );
}
