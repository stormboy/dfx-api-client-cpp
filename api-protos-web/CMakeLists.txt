set(CONAN_IMPORTED_WEB_PROTO_PATH ${CMAKE_BINARY_DIR}/protos-web)

add_library(
  api-protos-web OBJECT
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/analysis.proto
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/bundles.proto
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/devices.proto
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/general.proto
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/groups.proto
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/measurements.proto
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/meta.proto
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/organizations.proto
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/profiles.proto
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/studies.proto
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/users.proto
  ${CONAN_IMPORTED_WEB_PROTO_PATH}/dfx/proto/util.proto)

if(EXISTS ${CMAKE_SOURCE_DIR}/api-protos-web/dfx/proto/results.proto)
  target_sources(api-protos-web PUBLIC ${CMAKE_SOURCE_DIR}/api-protos-web/dfx/proto/results.proto)
  add_definitions(-DHAVE_DECODER_SUPPORT)
endif()

# Silence the Microsoft compiler warnings C4996/STL4015, related to the generated gRPC code when building with
# C++17
if(MSVC)
  if(CMAKE_CXX_STANDARD GREATER_EQUAL 17)
    target_compile_definitions(api-protos-web
                               PUBLIC "-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING")
  endif()
  target_compile_options(api-protos-web PUBLIC /utf-8)
endif()

target_link_libraries(api-protos-web protobuf::protobuf)

target_include_directories(
  api-protos-web
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         # Pick up the WebServiceDetails and share with our dependents
         $<BUILD_INTERFACE:$<TARGET_PROPERTY:dfxprotosweb::dfxprotosweb,INTERFACE_INCLUDE_DIRECTORIES>>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

# If building with tests (test-cpp-api), leave these symbols visible. In release mode, they should be hidden
# but when writing tests having access to raw protobufs can be helpful. ld: warning: direct access in function
# ... means the weak symbol cannot be overridden at runtime. This was likely caused by different translation
# units being compiled with different visibility settings. set_target_properties(api-protos-web PROPERTIES
# CXX_VISIBILITY_PRESET "default")

set_target_properties(api-protos-web PROPERTIES CXX_VISIBILITY_PRESET hidden)

# Make sure we source the correct definition from our overloaded CMAKE_MODULE_PATH, CMake has a definition of
# this, but we want to include now that we have defined where to look for it in order to get our correct
# protoc
include(protobuf-generate)

set(_protobuf_include_path -I ${CONAN_IMPORTED_WEB_PROTO_PATH} -I ${CMAKE_SOURCE_DIR}/api-protos-web)

protobuf_generate(TARGET api-protos-web)
