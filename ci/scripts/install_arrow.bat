mkdir C:\tmp
pushd C:\tmp
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg install arrow:x64-windows
.\vcpkg\vcpkg integrate install
popd
