@rem The vcpkg install for Apache Arrow does not include
@rem the Python libraries we need, so need to build ourselves
cd arrow\cpp
mkdir build
cd build
cmake ^
    - G "Visual Studio 15 2017" -A x64 ^
    -DARROW_DEPENDENCY_SOURCE=VCPKG ^
    -DARROW_COMPUTE=ON ^
    -DARROW_PYTHON=ON ^
    .. || exit /B 1
cmake --build . --target install --config Release || exit /B 1
