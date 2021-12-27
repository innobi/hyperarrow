set -e

# TODO: we can likely take arguments for version, platform, etc...
# but right now we don't know the git hash being used by Tableau for the C++
# source
cpp_url="https://downloads.tableau.com/tssoftware/tableauhyperapi-cxx-macos-x86_64-release-hyperapi_release_26.0.0.13821.r1fbe38ce.zip"

mkdir /tmp/tableau
wget -q ${cpp_url} -O tmp.zip
unzip -q tmp.zip -d /tmp/tableau
rm tmp.zip

pushd /tmp/tableau
cp -r tableauhyperapi-cxx-macos-x86_64-release-hyperapi_release_26.0.0.13821.r1fbe38ce/* .
rm -rf tableauhyperapi-cxx-macos-x86_64-release-hyperapi_release_26.0.0.13821.r1fbe38ce
popd

pypi_url="https://files.pythonhosted.org/packages/80/9d/d592d3bd06fa725d8a6834ae8a1ebd7b082ddf8eb90f0c4609e5eb91fced/tableauhyperapi-0.0.13980-py3-none-manylinux2014_x86_64.whl"

# Here we replace the normally pre-compiled lib / executable with one known
# to work on manylinux
mkdir /tmp/tableau-python
pushd /tmp/tableau-python
wget -q ${pypi_url} -O tableauhyperapi-0.0.13980-py3-none-manylinux2014_x86_64.whl
python -m wheel unpack tableauhyperapi-0.0.13980-py3-none-manylinux2014_x86_64.whl
rm -rf /tmp/tableau/lib/*
mv tableauhyperapi-0.0.13980/tableauhyperapi/bin/* /tmp/tableau/lib/
popd

rm -rf /tmp/tableau-python
