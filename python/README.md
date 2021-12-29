# Example Usage

```python
import hyperarrow as hal
import pyarrow as pa

tbl = pa.Table.from_pydict({"a": range(100)})
hal.write_to_hyper(tbl, "example.hyper", "schema", "table")
print(hal.read_from_hyper("example.hyper", "schema", "table")
```

# Building Wheels

Unix wheels are available from the top level of the repository using docker. The following commands will generate a wheel file in python/dist.

```sh
docker-compose build python-wheel-manylinux-2014
docker-compose up
```

Given the current state of macOS you unfortunately cannot use docker. See the github action for an example of how to build this wheel.

Windows is not yet implemented. Expected mid-Jan to early-Feb 2022.

