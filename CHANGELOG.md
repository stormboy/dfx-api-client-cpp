# Changelog
All notable changes to this project will be documented in this file.

## [1.3.1]
 - Fixed bug in WebSocket MeasurementStreamAPI.waitForCompletion when
   timeout was not the default wait forever value.

## [1.3.0]
 - Added support for passing WebSocket MeasurementStream properties
   during stream setup for things like resolution and streaming.
 - Bug fixed uninitialized WebSocket MeasurementResult

## [1.2.2]
 - Updated sample configuration to use public API endpoint api.deepaffex.ai
   operating on port 443.
 - Added support for CloudAPI::getTransportType() to determine the
   actual type of client instantiated, useful for filtering test cases.

## [1.2.1]
 - Removed enum device type LINUX, conflicts with required Visage
   macro definition on Linux

## [1.2.0]
 - Added base64 support for handling encoded study data
 - Updated to use "proto" websocket format
 - Updated to newer openssl

## [1.1.1]
 - Updated protobuf version to resolve grpc protobuf conflict
 - Updated CMake minimimum required format to the latest 3.23.1

## [1.1.0]
 - Added User support for retrieve/update operating on self when the
   id and email are not provided. When id and email are provided, the
   API uses the Organization end-points to update another user but
   requires connected with the appropriate ROLE.
 - Added dfxcli get user flag -s to operate on self
 - Fixed issue with User DateOfBirth which was inconsistently handled
   between WebSocket/REST (string) and gRPC (epoch). gRPC now uses
   the same field and uses the string representation.
 - Updated the conanfile to pull the version from the CMakeLists to
   ensure version is known consistently to CMake when constructing
   library.
 - Added CMake configure install/configure packages so package can
   be installed and found using find_package in another package.
 - CMakeLists dependencies more controlled and created separate
   projects for dfxcli and test, both of which can be build standalone.
 - Cleaned the library name up to be known as "libdfxcloud" matching
   find_package.

## [1.0.0]
 - Initial release
