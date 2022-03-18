# Licenses
Licenses are dependent on build options and what features are included, subject to their own licensing
constraints. This dfx-cloud library is licensed under an MIT license.  

## Base Runtime Required
- [libcurl](https://curl.se/docs/copyright.html) [~MIT], [source](https://github.com/curl/curl)
- [zlib](https://en.wikipedia.org/wiki/Zlib_License) [zlib], [source](https://github.com/madler/zlib)
- [fmt](https://github.com/fmtlib/fmt/blob/master/LICENSE.rst) [MIT], [source](https://github.com/fmtlib/fmt)
- [nlohmann_json](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT) [MIT], [source](https://github.com/nlohmann/json)
- [openssl](https://www.openssl.org/source/license-openssl-ssleay.txt) [OpenSSL, SSLeay], [source](https://github.com/openssl/openssl/tree/OpenSSL_1_1_1-stable)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp/blob/master/LICENSE) [MIT], [source](https://github.com/jbeder/yaml-cpp)

### REST Transport Additional
None, libcurl is present as a base requirement

### WebSocket Transport Additional
- [libwebsockets](https://libwebsockets.org/git/libwebsockets/tree/LICENSE) [MIT, +deps], [source](https://libwebsockets.org/git/libwebsockets)
- [Protobuf](https://github.com/protocolbuffers/protobuf/blob/master/LICENSE) [BSD], [source](https://github.com/protocolbuffers/protobuf)

### gRPC Transport Additional
- [Protobuf](https://github.com/protocolbuffers/protobuf/blob/master/LICENSE) [BSD], [source](https://github.com/protocolbuffers/protobuf)
- [grpc](https://github.com/grpc/grpc/blob/master/LICENSE) [APL2,BSD3,MPL2], [source](https://github.com/grpc/grpc)
- [googleapis](https://github.com/googleapis/googleapis/blob/master/LICENSE) [APL2], [source](https://github.com/googleapis/googleapis)
- [abseil](https://github.com/abseil/abseil-cpp/blob/master/LICENSE) [APL2], [source](https://github.com/abseil/abseil-cpp)
- [re2](https://github.com/google/re2/blob/main/LICENSE) [BSD3], [source](https://github.com/google/re2)
- [meson](https://github.com/mesonbuild/meson/blob/master/COPYING) [APL2], [source](https://github.com/mesonbuild/meson)
- [c-ares](https://c-ares.org/license.html) [MIT], [source](https://github.com/c-ares/c-ares)

---
## Building Documentation
- [Doxygen](https://www.doxygen.nl/index.html) [GPL], [source](https://github.com/doxygen/doxygen)
- [proto-matlab-doxygen](https://github.com/aphysci/proto-matlab-doxygen/blob/master/license.txt) [BSD], [source](https://github.com/aphysci/proto-matlab-doxygen)

## Building CLI tool
For the command line client dfxcli test program, it makes use of the following libraries.
- [CLI11](https://github.com/CLIUtils/CLI11/blob/main/LICENSE) [BSD3], [source](https://github.com/CLIUtils/CLI11)
- [naturalsort](https://github.com/scopeInfinity/NaturalSort/blob/master/LICENSE.md) [MIT], [source](https://github.com/scopeInfinity/NaturalSort)

## Building Tests
- [gtest](https://github.com/google/googletest/blob/main/LICENSE) [BSD3], [source](https://github.com/google/googletest)
- [gflags](https://github.com/gflags/gflags/blob/master/COPYING.txt) [BSD3], [source](https://github.com/gflags/gflags)


