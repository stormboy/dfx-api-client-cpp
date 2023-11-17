# Building

## Environment Setup

This package relies on the dependency management from [Conan](https://conan.io/)
and the [CMake](https://cmake.org/) build system. It needs *both* Conan and
CMake to build the library, but can be built by either Conan or CMake. Deciding
if Conan or CMake should build the library is related to how you will consume
the library. We also recommend that you use
[Just](https://github.com/casey/just) to automate the build steps along with
[nushell](https://github.com/nushell/nushell) to avoid the need of handling
platform script differences.

*Conan* is a tool for managing C++ dependencies and if you are writing an
application which depends on this library and is using Conan already, it is
trivial to add as an external dependency which handles the cross-platform and
versioning aspects for you.

If you are working on this library, while you could build with Conan it would be
a very slow work flow. Conan will cache the build when it's built so dependent
applications don't need to continuously rebuild, but when working on the library
itself you will likely make a lot of small changes and build and test. For this,
having a conventional working copy with pre-built and cached object files is
more efficient which makes building with CMake a better alternative.

If you are cross-compiling this library, it is easiest to do from Conan which is
done by selecting a profile which sets up the appropriate CMake Tool Chain.

Conan/CMake/Just installation is easiest done with a platform package manager,
but can be done via source as well:

### macOS (using https://brew.sh/)

```bash
brew install conan
brew install cmake
brew install just
brew install nu
```

### Windows (using https://scoop.sh/)

```bash
scoop bucket add main
scoop install main/conan   # OR choco install conan OR winget install conan
scoop install main/cmake   # OR choco install cmake OR winget install cmake
scoop install main/just    # OR manually download and place Just in your PATH
scoop install main/nu      # OR manually download and place nu in your PATH
```

### Ubuntu

```bash
sudo apt-get install build-essential libssl-dev  # Ensure compilers and SSL libraries
mkdir -p development && cd development # Replace `development` with you preferred directory
python3 -m venv venv         # Create Python virtual environment
source venv/bin/activate     # Activate virtual environment
pip install conan            # Install Conan in the virtual environment
pip install cmake            # Install CMake in the virtual environment

 # Assuming ~/.local/bin is in PATH, install just and nu
wget -O - https://github.com/casey/just/releases/download/1.14.0/just-1.14.0-x86_64-unknown-linux-musl.tar.gz 2>/dev/null | \
          tar -xz -C ~/.local/bin just

wget -O - https://github.com/nushell/nushell/releases/download/0.82.0/nu-0.82.0-x86_64-unknown-linux-musl.tar.gz 2>/dev/null | \
          tar -xz -C ~/.local/bin --strip-components=1 nu-0.82.0-x86_64-unknown-linux-musl/nu
```

## Building with `Just`

If you are using `Just` then it can perform all the subsequent steps for you.
You will need to invoke it from a Unix shell with access to the compilers on
your platform. (On Windows, this would be the Git Bash shell.)

```bash
just export      # Export all the conan required conan packages to conan cache

just build       # Build library (default type=Debug)

just             # Display help list of defaults/options

just type=Release build    # Build a Release build
just release     # Short cut for type=Release

just version     # Display current library version
just -s version  # Show steps in version recipe

just shared=False type=Release build  # Build a static, release library
```

Expect this to take a while. Please see the [Build Artifacts](#build-artifacts)
section below to find the artifacts that you built.

If you don't want to use `Just`, you can follow the steps below, manually.

## Building manually using Conan and CMake (alternative to `Just`)

### Installing dependencies using Conan

The most basic way to install dependencies is using standard Conan `install`
syntax. Expect this to take a while and can be built for both Debug & Release
targets.

```bash
conan install . -pr:b=default -o dfxcloud:enable_checks=False -if build --build missing -s build_type=Debug
conan install . -pr:b=default -o dfxcloud:enable_checks=False -if build --build missing -s build_type=Release
```

This places the built artifact into the Conan cache (~/.conan/data) and may take
a while if none of the upstream dependencies are available locally or from a
Conan server as a pre-built package. Subsequent builds will be much faster,
provided your dependency and option graphs remain consistent. This also readies
the `build` directory for the next step by placing the CMAKE_TOOLCHAIN_FILE
in it, so switch to it.

```bash
cd build
```

### Configuring and building using CMake

On Windows, CMake offers a number of different
[cmake-generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)
which can be used to create Ninja build files, Xcode Projects or Visual Studio
projects (VS 6 through VS 16 (2019)) from the CMakeLists.txt definitions.

For example, to create a Visual Studio 16 2019 for Win32, x64, ARM or ARM64 you
would use one of the following and then open the generated solution files.

```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" -A x64
```

On non-Windows platforms:

```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
```

Then you can build using,

```bash
cmake --build . --config Release

# On Windows
cmake --build . --config Release --target ALL_BUILD
```

## Build artifacts

You can find the library that was built in `build/Release/`

You can find some additional header files you will need in `build/include/`

You can find some optional resources in `build/res/`

You can use these in your own projects.

## Troubleshooting build issues

If you force a clean build of everything, ignoring pre-built artifacts it can
take about 60 minutes or more. This is useful if your platform is older then
what the artifacts were originally built on and you want to avoid warnings like
this macOS one,

```shell
  object file . . . was built for newer macOS version (10.11) than being linked (10.9)
```

Building everything can be done by omitting the `missing` option to build:

```shell
conan install dfxcloud/1.0.0@ --build
```

Alternatively, you can be more selective and if you know the specific libraries
which should be rebuilt, you can use a command like,

```shell
conan install dfxcloud/1.0.0@ --build openssl --build dfxcloud
```

which would build both `openssl` and `dfxcloud`.
