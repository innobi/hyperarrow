@rem Licensed to the Apache Software Foundation (ASF) under one
@rem or more contributor license agreements.  See the NOTICE file
@rem distributed with this work for additional information
@rem regarding copyright ownership.  The ASF licenses this file
@rem to you under the Apache License, Version 2.0 (the
@rem "License"); you may not use this file except in compliance
@rem with the License.  You may obtain a copy of the License at
@rem
@rem   http://www.apache.org/licenses/LICENSE-2.0
@rem
@rem Unless required by applicable law or agreed to in writing,
@rem software distributed under the License is distributed on an
@rem "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
@rem KIND, either express or implied.  See the License for the
@rem specific language governing permissions and limitations
@rem under the License.

@echo on

echo "Building windows wheel..."

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
rm -rf C:\hyperarrow-build

echo "=== (%PYTHON_VERSION%) Building HyperArrow libraries ==="
set HYPER_PATH=C:\tmp\tableau\tableauhyperapi

python -m pip install wheel
mkdir C:\hyperarrow-build
pushd C:\hyperarrow-build

cmake ^
    -DCMAKE_PREFIX_PATH=%HYPER_PATH%\share\cmake ^
    -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake ^
    -DVCPKG_MANIFEST_DIR=C:\arrow\cpp ^
    -G "Visual Studio 15 2017" -A x64 ^
    C:\hyperarrow || exit /B 1
cmake --build . --config Release --target python || exit /B 1
popd

pushd C:\hyperarrow\python
@REM bundle the msvc runtime
cp "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.16.27012\x64\Microsoft.VC141.CRT\msvcp140.dll" hyperarrow
popd
