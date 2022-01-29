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
rmdir /S /Q C:\hyperarrow-build
rmdir /S /Q C:\hyperarrow\python\dist
rmdir /S /Q C:\hyperarrow\python\build
rmdir /S /Q C:\hyperarrow\python\repaired_wheels

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

pip install delvewheel
cd dist
for %%a in (*.whl) do (
  delvewheel repair --wheel-dir ..\repaired_wheels^
    %%a ^
    --add-path "C:\Program Files\arrow\bin;C:\hyperarrow-build\src\Release;C:\tmp\tableau\tableauhyperapi\bin;C:\vcpkg\packages\re2_x64-windows\bin;C:\vcpkg\packages\utf8proc_x64-windows\bin"

  @rem Might be a bug but delvewheel makes hyperarrow.lib
  @rem instead of hyperarrow\.lib
  cd ..\repaired_wheels
  wheel unpack %%a
  @REM TODO - don't hard code this path
  cd hyperarrow-0.0.1.dev0
  cp hyperarrow.libs/* hyperarrow
  rmdir /S /Q hyperarrow.libs
  @rem also need to place Hyper executable herein
  cp -r %HYPER_PATH%\bin\hyper hyperarrow\
  cd ..
  wheel pack hyperarrow-0.0.1.dev0
  rmdir /S /Q hyperarrow-0.0.1.dev0
)
popd
