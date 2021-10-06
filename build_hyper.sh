set -ex

NPROC=$(nproc)

pushd $HYPER_BUILD_DIR/examples

# Enable the CSV reader as it's used by the example third-party build
cmake .

make -j$NPROC
make install

popd
