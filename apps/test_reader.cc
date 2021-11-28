#include "hyperarrow/reader.h"
#include <arrow/api.h>
#include <iostream>

int main(int argc, char **argv) {
  auto result =
      hyperarrow::arrowTableFromHyper("example.hyper", "schema", "table");
  if (result.ok()) {
    auto table = result.ValueOrDie();
    auto printed = arrow::PrettyPrint(*table, {}, &std::cerr);
    return printed.ok();
  } else {
    return -1;
  }
}
