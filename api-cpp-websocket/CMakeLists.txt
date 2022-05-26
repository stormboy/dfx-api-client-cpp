if(NOT WITH_WEBSOCKET)
  return() # WebSockets is only supported if built WITH_WEBSOCKET
endif(NOT WITH_WEBSOCKET)

add_definitions(-DWITH_WEBSOCKET)

# Use an absolute reference here so that doxygen can locate in the doc context by target
set(API_CPP_WEBSOCKET_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/websocket/DeviceWebSocket.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/websocket/LicenseWebSocket.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/websocket/MeasurementWebSocket.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/websocket/MeasurementStreamWebSocket.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/websocket/OrganizationWebSocket.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/websocket/ProfileWebSocket.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/websocket/StudyWebSocket.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/websocket/UserWebSocket.hpp)

add_library(
  api-cpp-websocket OBJECT
  src/CloudWebSocket.cpp
  src/DeviceWebSocket.cpp
  src/LicenseWebSocket.cpp
  src/MeasurementWebSocket.cpp
  src/MeasurementStreamWebSocket.cpp
  src/OrganizationWebSocket.cpp
  src/ProfileWebSocket.cpp
  src/StudyWebSocket.cpp
  src/UserWebSocket.cpp
  ${API_CPP_WebSocket_HEADERS})

if(MSVC)
  if(BUILD_SHARED_LIBS)
    # Warning C4251 complaining about std::string on a DLL boundary
    target_compile_options(api-cpp-websocket PUBLIC /wd4251)
  endif()
  target_compile_options(api-cpp-websocket PUBLIC /utf-8)
endif()

set_target_properties(api-cpp-websocket PROPERTIES DEFINE_SYMBOL "dfxcloud_EXPORTS")

target_include_directories(
  api-cpp-websocket
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:$<TARGET_PROPERTY:websocket,INTERFACE_INCLUDE_DIRECTORIES>>
  PRIVATE $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-cpp,INTERFACE_INCLUDE_DIRECTORIES>>
          $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-cpp-validator,INTERFACE_INCLUDE_DIRECTORIES>>
          $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-protos-web,INTERFACE_INCLUDE_DIRECTORIES>>)

set_target_properties(api-cpp-websocket PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_link_libraries(
  api-cpp-websocket
  PRIVATE api-protos-web
          api-utils
          websocket
          $<$<NOT:$<PLATFORM_ID:Emscripten>>:CURL::libcurl>
          fmt::fmt
          nlohmann_json::nlohmann_json
          base64::base64)
