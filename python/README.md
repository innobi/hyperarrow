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
	
From here we can build a wheel and install the wheel to use the python binding.

```sh
python setup.py bdist_wheel
pip install dist/hyperarrow
```

```python
import hyperarrow.lib
```

