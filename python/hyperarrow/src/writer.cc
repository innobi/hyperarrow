#include "hyperarrow/writer.h"
#include "arrow/python/pyarrow.h"

///
/// Bridge function between the Python C module and hyperarrow
///
/// Returns 0 on success, -1 otherwise
///
extern "C" int writeToHyper(PyObject *table) {
  if (!arrow::py::is_table(table)) {
    return -1;
  }

  auto table = arrow::py::unwrap_table(table);
  // TODO: this should probably return some kind of status code instead of
  // just NULL
  arrowTableToHyper(table, "example.hyper");
  return 0;
}
