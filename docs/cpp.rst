C++ Core
========

The core language for HyperArrow is C++, matching what is offered upstream by the `Arrow <https://arrow.apache.org/docs/index.html>`_ library.

Build Dependencies
------------------

HyperArrow relies on the follow components:

  * A c++ compiler
  * Apache Arrow
  * `CMake <https://cmake.org>`_ as a build system
  * `Boost <https://www.boost.org>`_ (for testing)
  * The `Tableau Hyper API <https://help.tableau.com/current/api/hyper_api/en-us/>`_

Refer to your OS on how to install the components, or alternately use `conda <https://docs.conda.io/en/latest/>`_ to create a virtual environment.

.. code-block:: sh

   conda create -n hyperarrow-dev
   conda activate hyperarrow-dev
   conda install -c conda-forge "arrow-cpp>=6.0" boost-cpp compilers

Due to the nature of how the Tableau Hyper API is distributed, you will need to download that directly from the `Tableau website <https://www.tableau.com/support/releases/hyper-api/>`_  and place somwhere on your computer where CMake can find it. If CMake is unable to resolve the location, you should set an environemnt variable for ``TABLEAU_CMAKE_PATH`` to point to the CMake file provided by Tableau. On Unix-based systems this may look something like:

.. code-block:: sh

   export TABLEAU_CMAKE_PATH=<SOME_DIRECTORY_PATH>/tableauhyperapi/share/cmake

How to Build
------------

We suggest doing an out-of-core compliation of the libraries. Assuming you are at the root of the HyperArrow project you can perform the following steps to accomplish this.

.. code-block:: sh

   mkdir build
   pushd build

   cmake -S ..
   cmake --build .
   ctest
   popd


   

   
