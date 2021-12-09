# Python Installation

The README in the root folder of this project contains instructions for building the Python bindings as a platform wheel using CMake. After completing those steps, you could install the wheel using `pip install dist/hyperarrow` OR perhaps even better if you plan on developing the bindings run `python -m pip install -e .` from this directory to get an "editable install"

# Example Usage

```python
import hyperarrow.libhyperarrow as hal
import pyarrow as pa

tbl = pa.Table.from_pydict({"a": range(100)})
hal.write_to_hyper(tbl, "example.hyper", "schema", "table")
print(hal.read_from_hyper("example.hyper", "schema", "table")
```

