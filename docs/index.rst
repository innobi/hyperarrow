HyperArrow
==========

.. toctree::
   :hidden:


What is it?
-----------

``HyperArrow`` provides users with an easy to use library for converting from `Arrow <https://arrow.apache.org>`_ tables to Tableau's `Hyper <https://help.tableau.com/current/api/hyper_api/en-us/>`_ format. The core library is written in C++ but also offers a Python binding.

How is this different from pantab?
----------------------------------

Python users may be familiar with the `pantab <https://pantab.readthedocs.io/en/latest/>`_ library for reading / writing Hyper extracts and DataFrames. While similar in design, there are concrete differences between these libraries.

Data Types
__________

Pandas was built using the `NumPy <https://numpy.org>`_ type system. While this has served open source analytics communities well, it has some shortcomings that are worth noting.

For starters, integer types are not nullable in the NumPy type system. In the presence of ``NULL``, arrays are coered to a float type. Due to the nature of floating point arithmetic, this can lead to imprecise calculations after coercion. This also has undesirable effects when working with smaller types (say ``uint8_t``) that occupy a much smaller address space than ``double_t`` types in memory.

Pandas attempts to work around this with its own extended type system. *Nullable types* are often represented in pandas with capital letters (ex: Int64 is a nullable integer, int64 is not). While this may prevent issues with floating point inaccuracies, it introduces memory overhead, design complexity, and to date is not as natively optimized as standard NumPy arrays.

By contrast, Arrow has a simplified type system. Primitive types are all supported natively with or without nullability. Fortunately enough, this type system is also closer to what Hyper expects, requiring less ambigous rules for converting between the two formats.

Memory Efficiency
_________________

The Arrow library is built from the ground up with very tight control over its memory usage. Arrow has a relatively compact footprint, and theoretically also supports working with datasets that are larger than the host system's memory space (this is referred to as "out of core" data).

Pandas, by contrast, relies on the Python interpreter to manage its memory. It is entirely bounded by the host systems available memory, and additionally has less efficient implementations of particular data types. For example, Arrow can work with native buffers interpretable as string data. Pandas by contrast, requires strings to be stored as Python objects. This not only consumes more memory for storage, but also yields less than ideal performance given the amount of dereferencing pandas must due to retrieve string objects.

Language Support
________________

Pandas is a Python package. Interoperability with other languages is very limited and sometimes even non-existant.

Arrow by contrast is developed in C++ and offers bindings to a large variety of langauges. This allows you to say seamlessly write one process in Java that shares a dataset with another program written in say Go. By using Arrow as a source for tabular data rather than pandas, we can similarly offer bindings in other non-Python languages that make it as simple as possible to serialize / deserialize data into the Hyper format.

Implementation Status
---------------------

In the core library, the following data types are fully serializable / deserializable to/from Hyper:

* int16
* int32
* int64
* float64
* boolean
* utf8 strings
* date32 (days since Unix epoch in Arrow)
* timestamp (includes aribtrary precision support)

Note for the ``timestamp`` type that serializing from Hyper extracts will yield a timestamp type with Microsecond precision, as that is what Hyper extracts support.

Languages supported are currently C++ and Python. If you would like to help contribute bindings in other languages please contact the developer.
  
