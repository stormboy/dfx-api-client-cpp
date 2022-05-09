# Changelog
All notable changes to this project will be documented in this file.

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
