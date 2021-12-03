# HyperArrow

## What is it?
This repository is the future home of *HyperArrow* - a library to allow seamless serialization between [Apache Arrow](https://arrow.apache.org/docs/index.html) Tables and Tableau's [Hyper API](https://help.tableau.com/current/api/hyper_api/en-us/index.html). This is in some regards functionally equivalent to what the [pantab](https://pantab.readthedocs.io/en/latest/) library offers, though the latter is limited to Python only. Given the cross-language bindings offered by Arrow, this project would open up tabular Hyper serialization to a much larger ecosystem.
	
## Local Build

For a local build you must show CMakeLists where it can find both a pre-built arrow library as well as the tableau hyper api.

```sh
mkdir build
pushd build
# Modify below to point to whereve the tableauhyperapi is
export TABLEAU_CMAKE_PATH=../tableauhyperapi/share/cmake

cmake ..
make
make test

# Optionally build python binding wheel in python/dist
make python
popd
```
