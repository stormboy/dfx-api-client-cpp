find_package(ZLIB CONFIG REQUIRED) # ZLIB::zlib

find_package(dfxprotobufspublic CONFIG REQUIRED) # dfxprotobufspublic::dfxprotobufspublic
find_package(dfxprotosweb CONFIG REQUIRED) # dfxprotosweb::dfxprotosweb
find_package(googleapis CONFIG REQUIRED) # googleapis::googleapis

find_package(nlohmann_json CONFIG REQUIRED) # nlohmann_json::nlohmann_json

# find_package(cmake CONFIG)
find_package(fmt CONFIG REQUIRED) # fmt::fmt
find_package(protobuf CONFIG REQUIRED) # protobuf::protobuf
find_package(yaml-cpp CONFIG REQUIRED) # yaml-cpp
if(NOT TARGET yaml-cpp::yaml-cpp)
  add_library(yaml-cpp::yaml-cpp ALIAS yaml-cpp)
endif()

if(WITH_GRPC)
  find_package(OpenSSL CONFIG REQUIRED) # OpenSSL::openssl
  find_package(absl CONFIG REQUIRED) # abseil::abseil
  find_package(re2 CONFIG REQUIRED) # re2::re2
  find_package(c-ares CONFIG REQUIRED) # c-ares::c-ares

  find_package(gRPC CONFIG REQUIRED) # gRPC::grpc++
endif(WITH_GRPC)

if(WITH_CURL)
  find_package(CURL CONFIG REQUIRED) # CURL::libcurl
endif(WITH_CURL)

if(WITH_WEBSOCKET)
  find_package(Libwebsockets CONFIG REQUIRED) # Libwebsockets::libwebsockets
  find_package(base64 CONFIG REQUIRED) # base64::base64
endif(WITH_WEBSOCKET)

# Handles importing files from the PACKAGE_FOLDERS, analogous to ConanFile::imports() stage. Since the conan
# imports() is broken for private requirements when using build contexts they are written here.
#
# It is actually more technically correct to be here anyway as the import task happens prior to the
# find_package which doesn't allow for overloading the package while this approach will.

# Conan creates find packages which are build type specific, this function will take which ever build type it
# finds and place it into a non-build type variable name if it hasn't been located already.
function(conan_get_package_folder package_name)
  if(NOT ${package_name}_PACKAGE_FOLDER)
    if(${package_name}_PACKAGE_FOLDER_DEBUG)
      set(${package_name}_PACKAGE_FOLDER
          ${${package_name}_PACKAGE_FOLDER_DEBUG}
          PARENT_SCOPE)
    elseif(${package_name}_PACKAGE_FOLDER_RELEASE)
      set(${package_name}_PACKAGE_FOLDER
          ${${package_name}_PACKAGE_FOLDER_RELEASE}
          PARENT_SCOPE)
    endif()
  endif()
endfunction()

function(conan_import_from_package package_name source destination)
  conan_get_package_folder(${package_name})
  if(NOT ${package_name}_PACKAGE_FOLDER)
    message(
      WARNING "Unable to locate ${package_name} PACKAGE FOLDER, skipping IMPORT ${source} -> ${destination}")
  else()
    file(COPY ${${package_name}_PACKAGE_FOLDER}/${source} DESTINATION ${destination})
  endif()
endfunction()

function(conan_replace_in_file file match replace)
  file(READ "${file}" READ_FILE_CONTENT)
  string(REPLACE "${match}" "${replace}" MODIFIED_FILE_CONTENT "${READ_FILE_CONTENT}")
  file(WRITE "${file}" "${MODIFIED_FILE_CONTENT}")
endfunction()

if(dfxprotobufspublic_FOUND)
  conan_import_from_package(dfxprotobufspublic protos/ ${CMAKE_BINARY_DIR}/protos-grpc)
endif(dfxprotobufspublic_FOUND)

if(dfxprotosweb_FOUND)
  conan_import_from_package(dfxprotosweb protos/ ${CMAKE_BINARY_DIR}/protos-web)
endif(dfxprotosweb_FOUND)

if(googleapis_FOUND)
  conan_import_from_package(googleapis protos/ ${CMAKE_BINARY_DIR}/protos-grpc)
endif(googleapis_FOUND)

if(libcurl_FOUND OR CURL_FOUND)
  # libcurl/res/cacert.pem there is one in gRPC called roots.pem but inexorgame does not package it up from
  # the build folder, so easier to use libcurl
  conan_import_from_package(libcurl res/ ${CMAKE_BINARY_DIR}/res)
endif(libcurl_FOUND OR CURL_FOUND)

if(protobuf_FOUND)
  conan_import_from_package(protobuf lib/cmake/protobuf/ ${CMAKE_BINARY_DIR}/cmake/)
  conan_replace_in_file(
    "${CMAKE_BINARY_DIR}/cmake/protobuf-generate.cmake" "\${protobuf_generate_LANGUAGE}_out "
    "\${protobuf_generate_LANGUAGE}_out=\${protobuf_generate_LANGUAGE_OPTIONS}")
endif(protobuf_FOUND)
