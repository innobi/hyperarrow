#include <iostream>
#include <arrow/api.h>
#include "hyperarrow/reader.h"

int main(int argc, char** argv) {
  auto result = hyperarrow::arrowTableFromHyper();
  if (result.ok()) {
    auto table = result.ValueOrDie();
    auto printed = arrow::PrettyPrint(*table, {}, &std::cerr);
    return printed.ok();
  } else {
    return -1;
  }
}
