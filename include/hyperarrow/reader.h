//===-- reader.h - reader definition for hyperarrow -------------*- C++ -*-===//
//
// Part of the HyperArrow Project, under the Apache License v2.0
// See https://github.com/innobi/hyperarrow/blob/main/LICENSE
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation to read Hyper files into Arrow tables
///
//===----------------------------------------------------------------------===//

#include <arrow/table.h>

namespace hyperarrow {
arrow::Result<std::shared_ptr<arrow::Table>>
arrowTableFromHyper(const std::string databasePath,
                    const std::string schemaName, const std::string tableName);
}
