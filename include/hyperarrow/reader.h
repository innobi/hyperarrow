#include <arrow/table.h>

namespace hyperarrow {
  arrow::Result<std::shared_ptr<arrow::Table>> arrowTableFromHyper();
}
