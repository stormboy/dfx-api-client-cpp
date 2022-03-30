if(NOT WITH_GRPC)
  return() # GRPC is only supported if built WITH_GRPC
endif(NOT WITH_GRPC)

set(CONAN_IMPORTED_GRPC_PROTO_PATH ${CMAKE_BINARY_DIR}/protos-grpc)

add_library(
  api-protos-grpc OBJECT
  # Imported protos from conan package googleapis which DFX
  # Cloud protos use
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/google/api/annotations.proto
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/google/api/http.proto
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/auth/v1/auth.proto
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/devices/v2/devices.proto
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/organizations/v1/organizations.proto
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/signals/v1/signals.proto
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/signals/v2/signals.proto
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/studies/v1/studies.proto
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/studysignals/v1/studysignals.proto
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/studysignals/v2/studysignals.proto
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/users/v1/users.proto
  ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/users/v2/users.proto)

# Silence the Microsoft compiler warnings C4996/STL4015, related to the generated gRPC code when building with
# C++17
if(MSVC AND CMAKE_CXX_STANDARD GREATER_EQUAL 17)
  target_compile_definitions(api-protos-grpc
                             PUBLIC "-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING")
  target_compile_options(api-protos-grpc PUBLIC /utf-8)
else()
  # Ignore Protobuf password deprecations warning: 'password' is deprecated [-Wdeprecated-declarations]
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    target_compile_options(api-protos-grpc PUBLIC -Wno-deprecated-declarations)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(api-protos-grpc PUBLIC -Wno-deprecated-declarations)
  endif()
endif()

# If building with tests (test-cpp-api), leave these symbols visible. In release mode, they should be hidden
# but when writing tests having access to raw protobufs can be helpful. ld: warning: direct access in function
# ... means the weak symbol cannot be overridden at runtime. This was likely caused by different translation
# units being compiled with different visibility settings. set_target_properties(api-protos-grpc PROPERTIES
# CXX_VISIBILITY_PRESET "default")

set_target_properties(api-protos-grpc PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_sources(
  api-protos-grpc
  PRIVATE ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/measurements/v2/measurements.proto
          ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/error/v1/error.proto
          ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/action/v1/action.proto
          ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/chunk/v1/chunk.proto
          ${CONAN_IMPORTED_GRPC_PROTO_PATH}/dfx/status/v1/status.proto
          ${CONAN_IMPORTED_GRPC_PROTO_PATH}/google/rpc/code.proto
          ${CONAN_IMPORTED_GRPC_PROTO_PATH}/google/rpc/status.proto
          ${CONAN_IMPORTED_GRPC_PROTO_PATH}/google/rpc/error_details.proto)

target_link_libraries(api-protos-grpc protobuf::protobuf $<$<BOOL:${WITH_GRPC}>:gRPC::grpc++>)

target_include_directories(api-protos-grpc PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

# Make sure we source the correct definition from our overloaded CMAKE_MODULE_PATH, CMake has a definition of
# this, but we want to include now that we have defined where to look for it in order to get our correct
# protoc
include(protobuf-generate)

set(_protobuf_include_path -I ${CONAN_IMPORTED_GRPC_PROTO_PATH})

protobuf_generate(TARGET api-protos-grpc)

if(WITH_GRPC)
  # find_package, will reset the protobuf-generate inclusion - we really want ours
  include(protobuf-generate)
  if(WITH_TESTS)
    # https://grpc.github.io/grpc/cpp/md_doc_unit_testing.html
    set(protobuf_generate_LANGUAGE_OPTIONS "generate_mock_code=true:")
  endif(WITH_TESTS)

  find_program(
    GRPC_CPP_PLUGIN_PROGRAM
    NAMES grpc_cpp_plugin
    PATHS ENV PATH
    NO_DEFAULT_PATH)

  protobuf_generate(
    TARGET
    api-protos-grpc
    LANGUAGE
    "grpc"
    PLUGIN
    "protoc-gen-grpc=${GRPC_CPP_PLUGIN_PROGRAM}"
    GENERATE_EXTENSIONS
    .grpc.pb.h
    .grpc.pb.cc)
endif(WITH_GRPC)
