if(NOT WITH_WEBSOCKET)
  return() # WebSockets is only supported if built WITH_WEBSOCKET
endif(NOT WITH_WEBSOCKET)

add_definitions(-DWITH_WEBSOCKET)

set(WEBSOCKET_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/dfx/websocket/WebSocket.hpp)

add_library(
  websocket OBJECT
  src/WebSocket.cpp $<$<NOT:$<BOOL:${EMSCRIPTEN}>>:src/WebSocketLibWebSocket.cpp>
  $<$<NOT:$<BOOL:${EMSCRIPTEN}>>:include/dfx/websocket/WebSocketLibWebSocket.hpp> ${WEBSOCKET_HEADERS})

target_include_directories(websocket PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

set_target_properties(websocket PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_link_libraries(websocket $<$<NOT:$<BOOL:${EMSCRIPTEN}>>:websockets>)
