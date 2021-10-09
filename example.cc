// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include <iostream>

#include <arrow/builder.h>
#include <arrow/csv/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <arrow/pretty_print.h>
#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/table.h>

using arrow::Status;

namespace {

  std::shared_ptr<arrow::Int64Array> BuildData() {
    arrow::Int64Builder builder;
    builder.Resize(8);
    std::vector<bool> validity = {true, true, true, false, true, true, true, true};
    std::vector<int64_t> values = {1, 2, 3, 0, 5, 6, 7, 8};
    builder.AppendValues(values, validity);

    std::shared_ptr<arrow::Int64Array> array;
    arrow::Status st = builder.Finish(&array);
    if (!st.ok()) {
      // handle error here
    }

    return array;
  }  

Status RunMain(int argc, char** argv) {
  const char* csv_filename = "test.csv";
  const char* arrow_filename = "test.arrow";

  std::cerr << "* Reading CSV file '" << csv_filename << "' into table" << std::endl;
  ARROW_ASSIGN_OR_RAISE(auto input_file,
                        arrow::io::ReadableFile::Open(csv_filename));
  ARROW_ASSIGN_OR_RAISE(
      auto csv_reader,
      arrow::csv::TableReader::Make(arrow::io::default_io_context(),
                                    input_file,
                                    arrow::csv::ReadOptions::Defaults(),
                                    arrow::csv::ParseOptions::Defaults(),
                                    arrow::csv::ConvertOptions::Defaults()));
  ARROW_ASSIGN_OR_RAISE(auto table, csv_reader->Read());

  std::cerr << "* Generating data:" << std::endl;
  std::shared_ptr<arrow::Int64Array> array = BuildData();

  std::cerr << "Here is the data:" << std::endl;
  for (auto i = 0; i < array->length(); i++) {
    if (array->IsValid(i)) {
      std::cerr << array->Value(i) << std::endl;      
    } else {
      std::cerr << "Null Value!" << std::endl;
    }

  }

  //std::cerr << "* Writing table into Arrow Hyper file '" << arrow_filename << "'" << std::endl;

  return Status::OK();
}

}  // namespace

int main(int argc, char** argv) {
  Status st = RunMain(argc, argv);
  if (!st.ok()) {
    std::cerr << st << std::endl;
    return 1;
  }
  return 0;
}
