.. Copyright (c) 2022

Getting started
===============

This short guide explains how to get started with `DFX Cloud` library once you have installed setup
the build environment.

Compiling the project
---------------------

There are Justfile scripts setup to perform common tasks of building and testing which operate
pretty much like a standard Makefile might and should work on OSX, Ubuntu, Windows (with Git for Bash).

.. code:: bash

   just build                # Standard build, defaults to Debug
   just t=Release build      # Build a Release build
   just build test           # Chain build and test targets
   just conan                # Conan create to package cache
   just                      # List all targets
   just -s build             # Show content of build target

The longer details of building which Justfile is performing are:

.. code:: bash

    conan install . --build missing -if build -s build_type=Debug
    cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=conan/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
    make -j$(nproc)

    # Run the dfxcli tool
    tool-dfxcli/dfxcli

    # Run the Google tests
    test/test-cloud-api
