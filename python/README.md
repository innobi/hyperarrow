# Hyperarrow

Currently this python wrapper is used internally for testing as well as prototyping. It is unclear yet if this should be delivered as a standalone Python package or it if will be integrated into pantab.

# Building

```sh
conda create -y -n hyperarrow-dev -c conda-forge \
  pyarrow \
  python=3.8

conda activate hyperarrow-dev
pip install tableauhyperapi
```

.. note::

	There is currently a gap where we build the internal hyperrrow libraries with an arrow library that differs from the one we build in Python. This may cause issues in case of binary incompatabilities
	
```sh
python -m pip install -e .
# OR 
python setup.py bdist_wheel --py-limited-api=cp37
pip install dist/hyperarrow
```

```python
import hyperarrow.libhyperarrow
import pyarrow as pa

tbl = pa.Table.from_pydict({"a": range(100)})
hyperarrow.libhyperarrow.write_to_hyper(tbl)
```

