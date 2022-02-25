# HyperArrow

## What is it?
This repository is the future home of *HyperArrow* - a library to allow seamless serialization between [Apache Arrow](https://arrow.apache.org/docs/index.html) Tables and Tableau's [Hyper API](https://help.tableau.com/current/api/hyper_api/en-us/index.html). This is in some regards functionally equivalent to what the [pantab](https://pantab.readthedocs.io/en/latest/) library offers, though the latter is limited to Python only. Given the cross-language bindings offered by Arrow, this project would open up tabular Hyper serialization to a much larger ecosystem.
	
## Local Build

The easiest way to get started working with the C++ code is using docker.

```sh
docker-compose build hyperarrow-dev
```

After building the container you can enter:

```sh
docker-compose run --rm hyperarrow-dev
```

And once within set up an "out of core" build as follows:

```sh
mkdir build
cd build
cmake -DHYPERARROW_TESTING=ON ..
make -j
make test
```