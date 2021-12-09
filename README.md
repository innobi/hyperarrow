# HyperArrow

## What is it?
This repository is the future home of *HyperArrow* - a library to allow seamless serialization between [Apache Arrow](https://arrow.apache.org/docs/index.html) Tables and Tableau's [Hyper API](https://help.tableau.com/current/api/hyper_api/en-us/index.html). This is in some regards functionally equivalent to what the [pantab](https://pantab.readthedocs.io/en/latest/) library offers, though the latter is limited to Python only. Given the cross-language bindings offered by Arrow, this project would open up tabular Hyper serialization to a much larger ecosystem.
	
## Local Build

For an easy way to build this package across platforms, you can use conda to first build your development environment.

```sh
conda create -n hyperarrow-dev
conda activate hyperarrow-dev
conda install -c conda-forge "arrow-cpp>=6.0" boost-cpp compilers

# Optional step if you would like to build python bindings
conda install -c conda-forge pyarrow
```

With that out of the way, you will want to have the Tableau Hyper API available somewhere on your machine. Before the build you will want to set an environment variable pointing to this location (note that the method for setting an environment variable will depend on your platform)

```sh
export TABLEAU_CMAKE_PATH=<SOME_DIRECTORY_PATH>/tableauhyperapi/share/cmake
```

With those steps completed you can start the build process. It is recommended to do an out of source compliation with CMake, which would look as follows

```sh
mkdir build
pushd build

cmake -S ..
cmake --build .
ctest

# Optionally build python binding wheel in python/dist
cmake --build . --target python
popd
```
