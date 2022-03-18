============
Installation
============

Getting the source code
=======================

This library handles the communication with the DeepAffex
Cloud services and companion to the `DeepAffex Extraction SDK
<https://www.deepaffex.ai/developers-sdk>`_. It implements
the REST and WebSocket protocols necessary for performing
a DeepAffex measurement as documented by the `DeepAffex API
<https://dfxapiversion10.docs.apiary.io>`_ website.

.. code-block:: bash

       git clone https://github.com/nuralogix/dfx-api-client-cpp.git

.. _dfx-cloud-api-build-options:

Dependencies
============

There is a long list of dependencies which CMake uses ``find_package()`` to locate so these
libraries *could* be injected from another system location, but it is *strongly recommended*
to use ``conan`` to provide these dependencies as described in the next section on ``Build Environment Setup``.

+---------------+----------------------------------------------+-------------------------+
| Library       | Source                                       | Needed for              |
+===============+==============================================+=========================+
| libcurl       | https://github.com/curl/curl                 | Base                    |
+---------------+-----------+----------------------------------+-------------------------+
| zlib          | https://github.com/madler/zlib               | Base                    |
+---------------+-----------+----------------------------------+-------------------------+
| fmt           | https://github.com/fmtlib/fmt                | Base                    |
+---------------+-----------+----------------------------------+-------------------------+
| nlohmann_json | https://github.com/nlohmann/json             | Base                    |
+---------------+----------------------------------------------+-------------------------+
| openssl       | https://github.com/openssl/openssl           | Base                    |
+---------------+----------------------------------------------+-------------------------+
| yaml-cpp      | https://github.com/jbeder/yaml-cpp           | Base                    |
+---------------+----------------------------------------------+-------------------------+
| libwebsockets | https://libwebsockets.org                    | WebSocket               |
+---------------+----------------------------------------------+-------------------------+
| Protobuf      | https://github.com/protocolbuffers/protobuf  | WebSocket & gRPC        |
+---------------+----------------------------------------------+-------------------------+
| gRPC          | https://github.com/grpc/grpc                 | gRPC                    |
+---------------+----------------------------------------------+-------------------------+
| googleapis    | https://github.com/googleapis/googleapis     | gRPC                    |
+---------------+----------------------------------------------+-------------------------+
| abseil        | https://github.com/abseil/abseil-cpp         | gRPC                    |
+---------------+----------------------------------------------+-------------------------+
| re2           | https://github.com/google/re2                | gRPC                    |
+---------------+----------------------------------------------+-------------------------+
| meson         | https://github.com/mesonbuild/meson          | gRPC                    |
+---------------+----------------------------------------------+-------------------------+
| c-ares        | https://github.com/c-ares/c-ares             | gRPC                    |
+---------------+----------------------------------------------+-------------------------+
| Doxygen       | https://github.com/doxygen/doxygen           | Documentation           |
+---------------+----------------------------------------------+-------------------------+
| CLI11         | https://github.com/CLIUtils/CLI11            | dfxcli tool             |
+---------------+----------------------------------------------+-------------------------+
| naturalsort   | https://github.com/scopeInfinity/NaturalSort | dfxcli tool             |
+---------------+----------------------------------------------+-------------------------+
| gtest         | https://github.com/google/googletest         | Testing                 |
+---------------+----------------------------------------------+-------------------------+
| gflags        | https://github.com/gflags/gflags             | Testing                 |
+---------------+----------------------------------------------+-------------------------+

Build Environment Setup
=======================

  .. NOTE ::

    Building requires a **fully C++17-compliant** compiler.

This package relies on the dependency management from `Conan <https://conan.io>`_ and the
`CMake <https://cmake.org>`_ build system. It needs *both* Conan and CMake to build the
library, but can be built by either Conan or CMake. Deciding if Conan or CMake should
build the library is related to how you will consume the library.

Conan is a great tool for managing dependencies and if you are writing an application
which depends on this library and using Conan already, it is trivial to add as an
external dependency which handles the cross-platform and version updating for you.

If you are working on this library, while you could build with Conan it would be a
very slow work flow. Conan will cache the build when it's built so dependent
applications don't need to continuously rebuild, but when working on the library
you make a small change, build and test. For this, having a conventional working
copy with object files pre-built is more efficient which makes building with CMake
a better alternative.

If you are cross-compiling this library, it is easiest to do from Conan which is
done by selecting a profile which sets up the appropriate CMake Tool Chain.

Conan/CMake/Just installation is easiest done with a platform package manager,
but can be done via source as well:

.. code:: bash

   # OSX (using https://brew.sh/)
   brew install conan
   brew install cmake
   brew install just

   #############################################################

   # Windows (using https://scoop.sh/)
   scoop install conan
   scoop install cmake
   scoop install just

   #############################################################

   # Ubuntu
   mkdir development
   cd development
   python3 -m venv venv         # Create Python virtual environment
   source venv/bin/activate     # Activate virtual environment
   pip install conan

   # Install CMake via build (package manager not recent enough)
   sudo apt-get install build-essential libssl-dev
   wget https://github.com/Kitware/CMake/releases/download/v3.22.3/cmake-3.22.3.tar.gz
   tar -zxvf cmake-3.22.3.tar.gz
   cd cmake-3.22.3
   ./bootstrap
   make
   sudo make install

   wget https://github.com/casey/just/releases/download/1.1.0/just-1.1.0-x86_64-unknown-linux-musl.tar.gz
   tar xvzf just-1.1.0-x86_64-unknown-linux-musl.tar.gz just
   sudo mv just /usr/local/bin

Configuring conan
-----------------
To retrieve packages which are Nuralogix hosted you will need to setup the Conan remote,

.. code:: bash

   conan remote add nuralogix \
         https://artifactory.na-east.nuralogix.ai/artifactory/api/conan/conan

Quick install instructions
--------------------------
The most basic way to build is using standard Conan *install* syntax. Expect this to take
a while and can be built for both Debug & Release targets.

.. code:: bash

   conan install dfxcloud/1.0.0@ --build missing -rnuralogix -o build_type=Debug
   conan install dfxcloud/1.0.0@ --build missing -rnuralogix -o build_type=Release

This places the built artifact into the Conan cache (~/.conan/data) and may take a
while if none of the upstream dependencies are available locally or from a Conan
server as a pre-built package. Subsequent builds will be much faster, provided your
dependency and option graphs remain consistent.

CMake Generators
----------------
CMake offers a number of different `cmake-generators <https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html>`_
which can be used to create Ninja build files, Xcode Projects or Visual Studio
projects (VS 6 through VS 16 (2019)) from the CMakeLists.txt definitions.

For example, to create a Visual Studio 16 2019 for Win32, x64, ARM or ARM64 you
would use one of the following and then open the generated solution files.

.. code:: bash

   cmake -G "Visual Studio 16 2019" -A Win32 ..

   cmake -G "Visual Studio 16 2019" -A x64 ..

   cmake -G "Visual Studio 16 2019" -A ARM ..

   cmake -G "Visual Studio 16 2019" -A ARM64 ..

Troubleshooting build issues
----------------------------
If you force a clean build of everything, ignoring pre-built artifacts it can take
about 60 minutes or more depending upon your computer processor speeds.

Sometimes OSX reports errors like: "object file . . . was built for newer OSX version (10.11) than being linked (10.9)"

Which can be resolved by a rebuild omitting the ``missing`` conan build option:

.. code:: bash

   conan install dfxcloud/1.0.0@ --build

Alternatively, you can be more selective and if you know the specific libraries which should be rebuilt.

.. code:: bash

   conan install dfxcloud/1.0.0@ --build openssl --build dfxcloud

Would build both openssl and dfxcloud.
