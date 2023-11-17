if(NOT WITH_WEBSOCKET_JSON)
  return() # WebSocketsJson is only supported if built WITH_WEBSOCKET_JSON
endif(NOT WITH_WEBSOCKET_JSON)

add_definitions(-DWITH_WEBSOCKET_JSON)

# Use an absolute reference here so that doxygen can locate in the doc context by target
set(API_CPP_WEBSOCKET_JSON_HEADERS
    include/dfx/api/websocket/json/DeviceWebSocketJson.hpp
    include/dfx/api/websocket/json/LicenseWebSocketJson.hpp
    include/dfx/api/websocket/json/MeasurementWebSocketJson.hpp
    include/dfx/api/websocket/json/MeasurementStreamWebSocketJson.hpp
    include/dfx/api/websocket/json/OrganizationWebSocketJson.hpp
    include/dfx/api/websocket/json/ProfileWebSocketJson.hpp
    include/dfx/api/websocket/json/StudyWebSocketJson.hpp
    include/dfx/api/websocket/json/UserWebSocketJson.hpp)

add_library(
  api-cpp-websocket-json OBJECT
  src/CloudWebSocketJson.cpp
  src/DeviceWebSocketJson.cpp
  src/LicenseWebSocketJson.cpp
  src/MeasurementWebSocketJson.cpp
  src/MeasurementStreamJsonWebSocket.cpp
  src/OrganizationWebSocketJson.cpp
  src/ProfileWebSocketJson.cpp
  src/StudyWebSocketJson.cpp
  src/UserWebSocketJson.cpp
  ${API_CPP_WEBSOCKET_JSON_HEADERS})

if(MSVC)
  if(BUILD_SHARED_LIBS)
    # Warning C4251 complaining about std::string on a DLL boundary
    target_compile_options(api-cpp-websocket-json PUBLIC /wd4251)
  endif()
  target_compile_options(api-cpp-websocket-json PUBLIC /utf-8)
endif()

set_target_properties(api-cpp-websocket-json PROPERTIES DEFINE_SYMBOL "dfxcloud_EXPORTS")

target_include_directories(
  api-cpp-websocket-json
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:$<TARGET_PROPERTY:websocket,INTERFACE_INCLUDE_DIRECTORIES>>
  PRIVATE $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-cpp,INTERFACE_INCLUDE_DIRECTORIES>>
          $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-cpp-validator,INTERFACE_INCLUDE_DIRECTORIES>>
          $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-protos-web,INTERFACE_INCLUDE_DIRECTORIES>>)

set_target_properties(api-cpp-websocket-json PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_link_libraries(
  api-cpp-websocket-json
  PRIVATE api-protos-web
          api-utils
          websocket
          $<$<NOT:$<PLATFORM_ID:Emscripten>>:CURL::libcurl>
          fmt::fmt
          nlohmann_json::nlohmann_json
          base64::base64)
