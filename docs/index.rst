HyperArrow
==========

.. toctree::
   :hidden:

.. toctree::
   :hidden:

   python
   cpp
   changelog

``HyperArrow`` is a collection of libraries in different langauges that gives users an easy way to convert `Arrow <https://arrow.apache.org>`_ tables to Tableau's `Hyper <https://help.tableau.com/current/api/hyper_api/en-us/>`_ format. The core library is written in C++.

Currently support languages are C++ and Python. Contributions to add any other language would be very welcome. Please refer to each language's section on installation and usage notes.

Why HyperArrow?
---------------

HyperArrow looks to build off of the advantages of the `Apache Arrow <https://arrow.apache.org/docs/index.html>`_ library. While the current implementation is limited in scope to just a C++ and Python library dealing with singular arrow ``Table`` objects, in the future we envision:

  * Reading / Writing Hyper files from a large variety of langauges
  * Serializing and Deserializing data that is larger than memory (i.e. "out-of-core")
  * Support streaming data into Hyper files
