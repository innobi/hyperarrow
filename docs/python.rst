Python
======

Quick Install
-------------

.. code-block:: sh

   python -m pip install hyperarrow

You may also want to test your installation with pytest

.. code-block:: sh

   python -m pip install pytest
   python -c "import hyperarrow as hal; hal.test()"

Usage Example
-------------

Writing to a Hyper file
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import pyarrow as pa
   import hyperarrow as hal

   schema = pa.schema(
       [
           ("int16", pa.int16()),
           ("int32", pa.int32()),
           ("int64", pa.int64()),
           ("float64", pa.float64()),
           ("boolean", pa.bool_()),
           ("string", pa.utf8()),
           ("date", pa.date32()),
           ("datetime", pa.timestamp("us")),
       ]
   )

   tbl = pa.Table.from_pydict(
       {
           "int16": range(3),
           "int32": range(3),
           "int64": range(3),
           "float64": [1.0, 2.0, 3.0],
           "boolean": [True, False, True],
           "string": list("abc"),
           "date": [
               datetime.date(1900, 8, 15),
               datetime.date(1970, 1, 2),
               datetime.date(2021, 12, 23),
           ],
           "datetime": [
               datetime.datetime(1900, 8, 15, 12, 36, 0),
               datetime.datetime(1970, 1, 2, 22, 42, 17, 0),
               datetime.datetime(2021, 12, 23, 14, 32, 59, 0),
           ],
       },
       schema=schema,
   )

   hal.write_to_hyper(tbl, str(tmp_hyper), "schema", "table")


Reading from Hyper
~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import hyperarrow as hal

   hal.read_from_hyper(str(tmp_hyper), "schema", "table")


Comparison to pantab
--------------------

The TLDR of HyperArrow's advantage over pantab `pantab <https://pantab.readthedocs.io/en/latest/>`_ is that it provides more native type resolution, while also future-proofing and using an Arrow library that is expected to grow in usage and importance in the data space.


Data Types
~~~~~~~~~~

Pandas was built using the `NumPy <https://numpy.org>`_ type system. While this has served open source analytics communities well, it has some shortcomings that are worth noting.

For starters, integer types are not nullable in the NumPy type system. In the presence of ``NULL``, arrays are coered to a float type. Due to the nature of floating point arithmetic, this can lead to imprecise calculations after coercion. This also has undesirable effects when working with smaller types (say ``uint8_t``) that occupy a much smaller address space than ``double_t`` types in memory.

Pandas attempts to work around this with its own extended type system. *Nullable types* are often represented in pandas with capital letters (ex: Int64 is a nullable integer, int64 is not). While this may prevent issues with floating point inaccuracies, it introduces memory overhead, design complexity, and to date is not as natively optimized as standard NumPy arrays.

By contrast, Arrow has a simplified type system. Primitive types are all supported natively with or without nullability. Fortunately enough, this type system is also closer to what Hyper expects, requiring less ambigous rules for converting between the two formats.

Leveraging the Arrow Format
~~~~~~~~~~~~~~~~~~~~~~~~~~~

pantab is built off of pandas, which has its own internal memory representation that isn't widely compatible with other processes or langauges. Arrow as a project solves this amongst other things. You can read more about what Arrow does in the `Apache Arrow Documentation <https://arrow.apache.org/docs/index.html>`_.
