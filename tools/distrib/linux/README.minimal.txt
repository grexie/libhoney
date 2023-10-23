CONTENTS
--------

cmake       Contains CMake configuration files shared by all targets.

include     Contains all required Honeycomb header files.

libhoney_dll  Contains the source code for the libhoney_dll_wrapper static library
            that all applications using the Honeycomb C++ API must link against.

Release     Contains libhoney.so and other components required to run the release
            version of Honeycomb-based applications. By default these files should be
            placed in the same directory as the executable.

Resources   Contains resources required by libhoney.so. By default these files
            should be placed in the same directory as libhoney.so.


USAGE
-----

Building using CMake:
  CMake can be used to generate project files in many different formats. See
  usage instructions at the top of the CMakeLists.txt file.

Please visit the Honeycomb Website for additional usage information.

https://bitbucket.org/chromiumembedded/honey/
