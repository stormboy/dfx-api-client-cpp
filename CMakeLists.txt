cmake_minimum_required(VERSION 3.23.1 FATAL_ERROR)

# conanfile.py parses next line to determine version which feeds the version to everything
set(DFX_CLOUD_LIBRARY_VERSION 1.3.0)

project(
  dfxcloud
  VERSION ${DFX_CLOUD_LIBRARY_VERSION}
  LANGUAGES CXX C)

# Using (at least) C++11 Features: using to write type aliases like typedefs Using (at least) C++17 Features:
# std::filesystem nested namespaces
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Use environment, ie. export MAKEFLAGS="VERBOSE=1"
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CONAN_CMAKE_SILENT_OUTPUT ${CMAKE_VERBOSE_MAKEFILE})

set(CMAKE_POSITION_INDEPENDENT_CODE ON) # By default off for static, but users may want on

# Default to BUILD_TYPE = Release unless overridden on command line with CMAKE_BUILD_TYPE.
if((NOT DEFINED CMAKE_BUILD_TYPE) OR (CMAKE_BUILD_TYPE STREQUAL ""))
  set(BUILD_TYPE Release)
else()
  set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()

set(MEASUREMENT_ONLY
    FALSE
    CACHE BOOL "Build with ONLY support to perform measurement (smaller library)")

set(WITH_GRPC
    TRUE
    CACHE BOOL "Build with gRPC support")
set(WITH_CURL
    TRUE
    CACHE BOOL "Build with curl support")
set(WITH_REST
    TRUE
    CACHE BOOL "Build with REST support")
set(WITH_WEBSOCKET
    TRUE
    CACHE BOOL "Build with WebSocket support")
set(WITH_VALIDATORS
    TRUE
    CACHE BOOL "Build with Validation support")

set(WITH_YAML
    TRUE
    CACHE BOOL "Build the library with YAML configuration support")

set(WITH_TESTS
    TRUE
    CACHE BOOL "Build with Testing support")

set(WITH_DFXCLI
    TRUE
    CACHE BOOL "Build dfxcli executable tool")

set(WITH_DOCS
    FALSE
    CACHE BOOL "Build documentation")
set(DOC_LANGUAGE
    "english"
    CACHE STRING "Language to build documentation for: english, chinese")

set(WITH_CLANG_FORMAT
    FALSE
    CACHE BOOL "Enable clang format targets")

set(ENABLE_CHECKS
    FALSE
    CACHE BOOL "Enable source checks (clang-tidy)")

# While this might not be great for mobile, it is nice to provide an all-in-one library with no external
# dependencies. If you want a smaller library or already have a copy externally managed you can set this to
# False but you will need to inform the CloudConfig where your file is located (or provide it the content). If
# you are working with a standalone device, this can be safely turned off as well. The check for
# CMAKE_CROSSCOMPILING is necessary because cmake is compiling for the platform and will build the wrong type
# of executable to execute in the build context.
if(NOT CMAKE_SYSTEM_NAME STREQUAL CMAKE_HOST_SYSTEM_NAME)
  set(EMBED_CA_CERTS
      FALSE
      CACHE BOOL "Embed the CA CERTS into library?")
else()
  set(EMBED_CA_CERTS
      TRUE
      CACHE BOOL "Embed the CA CERTS into library?")
endif()

set(BUILD_SHARED_LIBS
    True
    CACHE BOOL "Build the shared library")
set(BUILD_DOC
    ON
    CACHE BOOL "Build the Doxygen DFX API Documentation")

# Define CMAKE_MODULE_PATH **AFTER** loading Conan to ensure proper search order
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_BINARY_DIR}/cmake # CMake scripts injected by
                                                                                   # Conan (for proto gen)
)

if(DEFINED CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  message(STATUS "CMAKE_INSTALL_PREFIX is not set\n" "Default value: ${CMAKE_INSTALL_PREFIX}\n"
                 "Will set it to ${CMAKE_BINARY_DIR}/install")
  set(CMAKE_INSTALL_PREFIX
      "${CMAKE_BINARY_DIR}/install"
      CACHE PATH "Where the library will be installed to" FORCE)
endif()

include(find_dependencies)

if(EMBED_CA_CERTS)
  include(embed_file)
endif(EMBED_CA_CERTS)

if(BUILD_SHARED_LIBS)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Force this to flow through to the OBJECT libraries
endif(BUILD_SHARED_LIBS)

# GNUInstallDirs to install libraries into correct locations on all platform
include(GNUInstallDirs)

add_subdirectory(api-utils)
add_subdirectory(api-protos-web)
add_subdirectory(api-protos-grpc)
add_subdirectory(api-cpp)
add_subdirectory(api-cpp-validator)
add_subdirectory(api-cpp-grpc)
add_subdirectory(api-cpp-rest)
add_subdirectory(websocket) # Provide an implementation for api-cpp-websockets to use
add_subdirectory(api-cpp-websocket)
add_subdirectory(doc)

add_library(dfxcloud "")
add_library(DFXCloud::dfxcloud ALIAS dfxcloud)

if(WITH_DFXCLI)
  add_subdirectory(tool-dfxcli)
endif(WITH_DFXCLI)

if(WITH_TESTS)
  add_subdirectory(test)
endif(WITH_TESTS)

if(MSVC AND CMAKE_BUILD_TYPE MATCHES DEBUG) # Microsoft compiler flag
  # LNK4099 PDB 'filename' was not found with 'object/library' or at 'path'; linking object as if no debug
  # info
  target_compile_options(dfxcloud PUBLIC /wd4099)
  set_target_properties(dfxcloud PROPERTIES LINK_FLAGS "/ignore:4099")
endif(MSVC AND CMAKE_BUILD_TYPE MATCHES DEBUG)

target_link_libraries(
  dfxcloud
  PUBLIC api-cpp api-utils
  PRIVATE protobuf::protobuf
          $<TARGET_OBJECTS:api-protos-web>
          $<$<BOOL:${WITH_GRPC}>:$<TARGET_OBJECTS:api-protos-grpc>>
          $<$<BOOL:${WITH_GRPC}>:$<TARGET_OBJECTS:api-cpp-grpc>>
          $<$<BOOL:${WITH_REST}>:$<TARGET_OBJECTS:api-cpp-rest>>
          $<$<BOOL:${WITH_WEBSOCKET}>:$<TARGET_OBJECTS:api-cpp-websocket>>
          $<$<BOOL:${WITH_WEBSOCKET}>:$<TARGET_OBJECTS:websocket>>)

# When not including gRPC, Undefined symbols for architecture arm64: "_CFRelease", referenced from:
# _Curl_resolv in libcurl.a(libcurl_la-hostip.o)
if(APPLE)
  target_link_libraries(dfxcloud PRIVATE "-framework CoreFoundation")
endif()

if(APPLE)
  # Some of generated protobuf files contain no symbols. tell ranlib to ignore empty compilation units
  set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols <TARGET>")
  set(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols <TARGET>")

  # prevents ar from invoking ranlib, let CMake do it
  set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qc -S <TARGET> <LINK_FLAGS> <OBJECTS>")
  set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qc -S <TARGET> <LINK_FLAGS> <OBJECTS>")
endif()

# Windows doesn't like IPO, enable only if supported
include(CheckIPOSupported) # CMake 3.9 - CMP0069 NEW
check_ipo_supported(RESULT ipoResult OUTPUT ipoOutput)
if(ipoResult)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

  # The ANDROID armv8 build uses optimization level O2 by default, but the armv7 build uses Oz and it fails to
  # link the LTO using the gold linker in the final stage due to bug:
  # https://github.com/android/ndk/issues/721
  if(ANDROID AND ANDROID_ABI STREQUAL "armeabi-v7a")
    string(REGEX REPLACE "-Oz" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  endif()
else()
  message(WARNING "IPO is not supported: ${ipoOutput}")
endif()

if(APPLE)
  set_target_properties(dfxcloud PROPERTIES INSTALL_RPATH "@executable_path/../lib")
elseif(WIN32)
  # NO @RPATH on Windows
else()
  set_target_properties(dfxcloud PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(NOT EMSCRIPTEN)
    if(APPLE)
      set_target_properties(
        dfxcloud PROPERTIES LINK_FLAGS
                            "-Wl,-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/cmake/clang-exports")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set_target_properties(
        dfxcloud PROPERTIES LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-exports")
    endif()
  endif()

  if(CMAKE_STRIP AND NOT EMSCRIPTEN)
    if(UNIX) # Same for both OSX and Linux
      add_custom_command(
        TARGET dfxcloud
        POST_BUILD
        COMMAND ${CMAKE_STRIP} -x $<TARGET_FILE:dfxcloud>)
    endif()
  endif()
endif()

# This will not work on Windows if generated against an object library as there is implicit CMake defines
# being injected for the real library being generated, in this case dfxcloud.
include(GenerateExportHeader)
generate_export_header(dfxcloud BASE_NAME DFXCLOUD EXPORT_FILE_NAME include/dfx/api/CloudAPI_Export.hpp)
add_definitions(-Ddfxcloud_EXPORTS)

install(
  TARGETS dfxcloud
  EXPORT DFXCloud-export
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER DESTINATION include-bad/)

install(
  EXPORT DFXCloud-export
  FILE DFXCloudTargets.cmake
  NAMESPACE DFXCloud::
  DESTINATION lib/cmake/DFXCloud)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DFXCloudConfig.cmake DESTINATION "lib/cmake/DFXCloud")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/dfx/api/CloudAPI_Export.hpp" DESTINATION include/dfx/api)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/DFXCloudConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/DFXCloudConfigVersion.cmake DESTINATION "lib/cmake/DFXCloud")

if(ENABLE_CHECKS)
  include(cmake/clang-tidy.cmake)
  include(cmake/clang-format.cmake)
else()
  if(WITH_CLANG_FORMAT)
    include(cmake/clang-format.cmake)
  endif()
endif()

message(STATUS "")
message(STATUS "----------------------")
message(STATUS "Building with options:")
message(STATUS "----------------------")
message(STATUS "")
message(STATUS "    Target Host:  ${CMAKE_SYSTEM_NAME}, Arch: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "    CXX COMPILER: ${CMAKE_CXX_COMPILER}    (${CMAKE_CXX_COMPILER_VERSION})")
message(STATUS "    CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "    CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "    BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message(STATUS "    SHARED_LIBS=${BUILD_SHARED_LIBS}")
message(STATUS "")
message(STATUS "    WITH_CURL=${WITH_CURL}")
message(STATUS "    WITH_REST=${WITH_REST}")
message(STATUS "    WITH_WEBSOCKET=${WITH_WEBSOCKET}")
message(STATUS "    WITH_GRPC=${WITH_GRPC}")
message(STATUS "    WITH_VALIDATORS=${WITH_VALIDATORS}")
message(STATUS "")
message(STATUS "    WITH_YAML=${WITH_YAML}")
message(STATUS "")
message(STATUS "    WITH_DOCS=${WITH_DOCS}  (${DOC_LANGUAGE})")
message(STATUS "    WITH_TESTS=${WITH_TESTS}")
message(STATUS "")
message(STATUS "    EMBED_CA_CERTS=${EMBED_CA_CERTS}")
message(STATUS "")

set(CMAKE_DUMP_PROPS OFF)
if(CMAKE_DUMP_PROPS)
  get_cmake_property(_variableNames VARIABLES)
  foreach(_variableName ${_variableNames})
    message(STATUS "CMAKE:${_variableName}=${${_variableName}}")
  endforeach()
endif(CMAKE_DUMP_PROPS)
