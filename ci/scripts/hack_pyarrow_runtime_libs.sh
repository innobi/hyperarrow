# See https://stackoverflow.com/questions/335928/ld-cannot-find-an-existing-library
# pyarrow only distributes runtime libs but the linker expects them without
# the appended version. We hack that here by creating symlinks to each

PYARROW_SO_VERSION=600
PYARROW_LIB_DIR=$(python -c "import pyarrow as pa; print(pa.get_library_dirs()[0])")

for file in ${PYARROW_LIB_DIR}/*.${PYARROW_SO_VERSION}; do
    NO_SUFFIX=${file%".$PYARROW_SO_VERSION"}
    ln -s ${file} ${NO_SUFFIX}
    echo "Created symlink ${NO_SUFFIX} pointing to ${file}"
done
