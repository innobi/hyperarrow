mkdir C:\tmp
pushd C:\tmp
git clone https://github.com/Microsoft/vcpkg.git
echo "Bootstrapping vcpkg"
call .\vcpkg\bootstrap-vcpkg.bat
echo "Running Arrow Install"
.\vcpkg\vcpkg install arrow:x64-windows
echo "Integrating vcpkg with Visual Studio"
.\vcpkg\vcpkg integrate install
popd
