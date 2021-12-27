#!/usr/bin/env bash
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

set -ex

echo "=== (${PYTHON_VERSION}) Clear output directories and leftovers ==="
# Clear output directories and leftovers
rm -rf /tmp/hyperarrow-build
rm -rf /hyperarrow/python/dist
rm -rf /hyperarrow/python/build
rm -rf /hyperarrow/python/repaired_wheels
rm -rf /hyperarrow/python/pyarrow/*.so
rm -rf /hyperarrow/python/pyarrow/*.so.*

echo "=== (${PYTHON_VERSION}) Building HyperArrow C++ libraries and Python ==="

mkdir /tmp/hyperarrow-build
pushd /tmp/hyperarrow-build
TABLEAU_CMAKE_PATH="/tmp/tableau/tableauhyperapi/share/cmake"
# TODO: don't hard code the PYTHON VERSION but note that the top
# level environment variable does not contain the "." we need
cmake \
  -DCMAKE_PREFIX_PATH=${TABLEAU_CMAKE_PATH} \
  -DCMAKE_PYTHON_VERSION="3.8" \
  /hyperarrow
make -j"$(nproc)"
make python
popd

pushd /hyperarrow/python
auditwheel repair -L . dist/hyperrrow-*.whl -w repaired_wheels
popd



