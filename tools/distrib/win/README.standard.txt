CONTENTS
--------

cmake       Contains CMake configuration files shared by all targets.

Debug       Contains libhoneycomb.dll, libhoneycomb.lib and other components required to
            build and run the debug version of Honeycomb-based applications. By
            default these files should be placed in the same directory as the
            executable and will be copied there as part of the build process.

include     Contains all required Honeycomb header files.

libhoneycomb_dll  Contains the source code for the libhoneycomb_dll_wrapper static library
            that all applications using the Honeycomb C++ API must link against.

Release     Contains libhoneycomb.dll, libhoneycomb.lib and other components required to
            build and run the release version of Honeycomb-based applications. By
            default these files should be placed in the same directory as the
            executable and will be copied there as part of the build process.

Resources   Contains resources required by libhoneycomb.dll. By default these files
            should be placed in the same directory as libhoneycomb.dll and will be
            copied there as part of the build process.

tests/      Directory of tests that demonstrate Honeycomb usage.

  honeyclient Contains the honeyclient sample application configured to build
            using the files in this distribution. This application demonstrates
            a wide range of Honeycomb functionalities.

  honeysimple Contains the honeysimple sample application configured to build
            using the files in this distribution. This application demonstrates
            the minimal functionality required to create a browser window.

  honeytests  Contains unit tests that exercise the Honeycomb APIs.

  gtest     Contains the Google C++ Testing Framework used by the honeytests
            target.

  shared    Contains source code shared by the honeyclient and honeytests targets.


USAGE
-----

Building using CMake:
  CMake can be used to generate project files in many different formats. See
  usage instructions at the top of the CMakeLists.txt file.

Please visit the Honeycomb Website for additional usage information.

https://bitbucket.org/chromiumembedded/honey/
