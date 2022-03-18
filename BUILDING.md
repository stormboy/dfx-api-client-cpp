# Build Environment Setup

This package relies on the dependency management from [Conan](https://conan.io/) and the
[CMake](https://cmake.org/) build system. It needs *both* Conan and CMake to build the
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

Conan/CMake/Just installation is easiest done with a platform package manager, but can be done via source as well:
```bash
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
```

### Configuring Conan
To retrieve packages which are Nuralogix hosted you will need to setup the Conan remote,
```bash
conan remote add nuralogix https://artifactory.na-east.nuralogix.ai/artifactory/api/conan/conan
```

The most basic way to build is using standard Conan *install* syntax. Expect this to take 
a while and can be built for both Debug & Release targets.
```bash
conan install dfxcloud/1.0.0@ --build missing -rnuralogix -o build_type=Debug
conan install dfxcloud/1.0.0@ --build missing -rnuralogix -o build_type=Release
```

This places the built artifact into the Conan cache (~/.conan/data) and may take a 
while if none of the upstream dependencies are available locally or from a Conan 
server as a pre-built package. Subsequent builds will be much faster, provided your 
dependency and option graphs remain consistent.



### CMake Generators
CMake offers a number of different
[cmake-generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)
which can be used to create Ninja build files, Xcode Projects or Visual Studio
projects (VS 6 through VS 16 (2019)) from the CMakeLists.txt definitions.

For example, to create a Visual Studio 16 2019 for Win32, x64, ARM or ARM64 you
would use one of the following and then open the generated solution files.
```bash
cmake -G "Visual Studio 16 2019" -A Win32 ..

cmake -G "Visual Studio 16 2019" -A x64 ..

cmake -G "Visual Studio 16 2019" -A ARM ..

cmake -G "Visual Studio 16 2019" -A ARM64 ..
```

### Troubleshooting build issues
If you force a clean build of everything, ignoring pre-built artifacts it can take
about 60 minutes or more. This is useful if your platform is older then what the
artifacts were originally built on and you want to avoid warnings like this OSX one,
```
  object file . . . was built for newer OSX version (10.11) than being linked (10.9)
```
Building everything can be done by omitting the ```missing``` option to build:
```
conan install dfxcloud/1.0.0@ --build
```
Alternatively, you can be more selective and if you know the specific libraries
which should be rebuilt.
```
conan install dfxcloud/1.0.0@ --build openssl --build dfxcloud
```
Would build both openssl and dfxcloud. 


