set -e

pypi_url="https://files.pythonhosted.org/packages/80/9d/d592d3bd06fa725d8a6834ae8a1ebd7b082ddf8eb90f0c4609e5eb91fced/tableauhyperapi-0.0.13980-py3-none-manylinux2014_x86_64.whl"

# Here we replace the normally pre-compiled lib / executable with one known
# to work on manylinux
mkdir /tmp/tableau-python
cd /tmp/tableau-python
wget -q ${pypi_url} -O tableauhyperapi-0.0.13980-py3-none-manylinux2014_x86_64.whl

python -m wheel unpack tableauhyperapi-0.0.13980-py3-none-manylinux2014_x86_64.whl
rm -rf /tmp/tableau/tableauhyperapi/lib/*
mv tableauhyperapi-0.0.13980/tableauhyperapi/bin/* /tmp/tableau/tableauhyperapi/lib/

rm -rf /tmp/tableau-python
