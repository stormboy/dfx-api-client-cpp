if(NOT WITH_WEBSOCKET_PROTOBUF)
  return() # WebSocket Protobuf is only supported if built WITH_WEBSOCKET_PROTOBUF
endif(NOT WITH_WEBSOCKET_PROTOBUF)

add_definitions(-DWITH_WEBSOCKET_PROTOBUF)

# Use an absolute reference here so that doxygen can locate in the doc context by target
set(API_CPP_WEBSOCKET_PROTOBUF_HEADERS
    include/dfx/api/websocket/protobuf/DeviceWebSocketProtobuf.hpp
    include/dfx/api/websocket/protobuf/LicenseWebSocketProtobuf.hpp
    include/dfx/api/websocket/protobuf/MeasurementWebSocketProtobuf.hpp
    include/dfx/api/websocket/protobuf/MeasurementStreamWebSocketProtobuf.hpp
    include/dfx/api/websocket/protobuf/OrganizationWebSocketProtobuf.hpp
    include/dfx/api/websocket/protobuf/ProfileWebSocketProtobuf.hpp
    include/dfx/api/websocket/protobuf/StudyWebSocketProtobuf.hpp
    include/dfx/api/websocket/protobuf/UserWebSocketProtobuf.hpp)

add_library(
  api-cpp-websocket-protobuf OBJECT
  src/CloudWebSocketProtobuf.cpp
  src/DeviceWebSocketProtobuf.cpp
  src/LicenseWebSocketProtobuf.cpp
  src/MeasurementWebSocketProtobuf.cpp
  src/MeasurementStreamWebSocketProtobuf.cpp
  src/OrganizationWebSocketProtobuf.cpp
  src/ProfileWebSocketProtobuf.cpp
  src/StudyWebSocketProtobuf.cpp
  src/UserWebSocketProtobuf.cpp
  ${API_CPP_WEBSOCKET_PROTOBUF_HEADERS})

if(MSVC)
  if(BUILD_SHARED_LIBS)
    # Warning C4251 complaining about std::string on a DLL boundary
    target_compile_options(api-cpp-websocket-protobuf PUBLIC /wd4251)
  endif()
  target_compile_options(api-cpp-websocket-protobuf PUBLIC /utf-8)
endif()

set_target_properties(api-cpp-websocket-protobuf PROPERTIES DEFINE_SYMBOL "dfxcloud_EXPORTS")

target_include_directories(
  api-cpp-websocket-protobuf
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:$<TARGET_PROPERTY:websocket,INTERFACE_INCLUDE_DIRECTORIES>>
  PRIVATE $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-cpp,INTERFACE_INCLUDE_DIRECTORIES>>
          $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-cpp-validator,INTERFACE_INCLUDE_DIRECTORIES>>
          $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-protos-web,INTERFACE_INCLUDE_DIRECTORIES>>)

set_target_properties(api-cpp-websocket-protobuf PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_link_libraries(
  api-cpp-websocket-protobuf
  PRIVATE api-protos-web
          api-utils
          websocket
          $<$<NOT:$<PLATFORM_ID:Emscripten>>:CURL::libcurl>
          fmt::fmt
          nlohmann_json::nlohmann_json
          base64::base64)
