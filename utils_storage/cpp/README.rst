Recording Service Storage plug-in: CSV Converter
************************************************

.. |RecS| replace:: *RecordingService*
.. |SP| replace:: *Storage plug-in*
.. |CSV| replace:: ``CSV``
.. |SD| replace:: *ShapesDemo*.

.. |br| raw:: html

   <br />

Building
========

Below there are the instructions to build and use this plug-in. All the commands
and syntax used assume a Unix-based system. If you run this example in a different
architecture, please adapt the commands accordingly.

To build the example you will need first to run CMake to generate the build files.
From the directory containing the example sources:

::

    mkdir build
    cd build
    cmake -DCONNEXTDDS_DIR=<Connext directory> \
            -DCONNEXTDDS_ARCH=<ARCH> \
            -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_SHARED_LIBS=ON ..
    cmake --build .


where:

- ``<Connext directory>`` shall be replaced with the path to the installation
  directory of *RTI Connext*.
- ``<ARCH>`` shall be replaced with the target architecture where you are
  running the example (e.g., x64Darwin15clang7.0).

.. note::

    For certain CMake generators you may need to provide the target platform
    through the ``-A`` command-line option. For example on Windows systems, you
    can provide the values ``Win32`` or ``x64``

.. note::

    Certain `CMake generators <https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html>`_
    generate multi-configuration files may need explicit selection of the
    configuration through ``--config``. For example for Visual Studio 2015, by
    default you can select ``Release`` or ``Debug``.

Upon success of the previous command it will create a shared library file in
the build directory.


Running
=======

To use this plug-in you will need to create a |RecS| configuration for the
*Converter* tool and select a custom storage. As part of this plug-in
distribution you can find an example file that selects and configures this
plug-in and converts all the *Topics* available in an input database in ``CDR``
format.

To run this example you will need to run *RTI Shapes Demo*  and |RecS| (for
*Recording* and *Converter* tools).

Setup
-----

If you run |RecS| from a different directory where the plug-in library is
located, you will need first to set up your environment to point to location
of the library. For example:

* Unix plaforms:

    ::

        export RTI_LD_LIBRARY_PATH=$PWD/Debug/x64

* Windows plaforms:

    ::

        set PATH=%PATH%;$PWD\Debug\x64

The steps to run the example in following section assumes you are running |RecS|
from the output build directory.

Execution
---------

#. Run one instance of |SD| on domain 0. This will be the publisher application.
   Publish blue squares and blue circles.


#. Run |RecS| to cause the recording data from the publisher application
   Run the following command from the example  build directory:

   ::

        <Connext directory>/bin/rtirecordingservice


   Run the service for a few seconds then press ``Ctrl+C`` to shutdown the service.
   On successful recording, you will find a directory with name ``cdr_recording``
   in the working directory containing the recorded shapes in a ``sqlite``
   database in ``CDR`` format.

#. Now run *Converter* to perform the conversion of the previously generated
   database into a single text file in |CSV| format:

   ::

        <Connext directory>/bin/rticonverter \
                -cfgFile ../ConverterToCSv.xml \
                -cfgName CdrToCsv


   Wait until conversion of the entire input database is perform and the
   application exits automatically. Upon successful conversion, you will
   find a file in the current directory with name ``ExampleCsv.csv`` that
   contains the converted content.
