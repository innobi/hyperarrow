# HyperArrow

## What is it?
This repository is the future home of *HyperArrow* - a library to allow seamless serialization between [Apache Arrow](https://arrow.apache.org/docs/index.html) Tables and Tableau's [Hyper API](https://help.tableau.com/current/api/hyper_api/en-us/index.html). This is in some regards functionally equivalent to what the [pantab](https://pantab.readthedocs.io/en/latest/) library offers, though the latter is limited to Python only. Given the cross-language bindings offered by Arrow, this project would open up tabular Hyper serialization to a much larger ecosystem.

## Current State
As of writing this project provides a Dockerfile that builds Arrow, downloads the Hyper API and builds a small example file with linkage to the former two. After downloading the project, you can run this with

```sh
dockerfile build -t hyperarrow .
```

The Dockerfile produces a sample hyper file located at ``/build/hyperarrow/example.hyper`` within the image. You can copy this locally via the following commands:

```sh
id=$(docker create hyperarrow)
docker cp $id:/build/hyperarrow/example.hyper - > example.hyper
docker rm -v $id
```

## Debug Mode

The repository is currently building in debug mode (see CMakeLists.txt and build_arrow.sh scripts referencing Debug mode). After building, if you start a docker container with --privileged command such as

```sh
docker run -it --privileged hyperarrow
```

You may subsequently run the main executable using lldb. As an example, to set a breakpoint in the code you would do something like:

```sh
lldb ./hyperarrow
breakpoint set --file src/hyperarrow/hyperarrow.cc --line 51
run
```

	
## Local Build

For a local build you must show CMakeLists where it can find both a pre-built arrow library as well as the tableau hyper api.

```sh
mkdir build
pushd build
export TABLEAU_CMAKE_PATH=<path_to_tableau>

cmake ..
popd
```
