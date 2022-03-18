if(NOT WITH_GRPC)
  return() # GRPC is only supported if built WITH_GRPC
endif(NOT WITH_GRPC)

# Use an absolute reference here so that doxygen can locate in the doc context by target
set(API_CPP_GRPC_PUBLIC_HEADERS
    ${CMAKE_SOURCE_DIR}/api-cpp-grpc/include/dfx/api/grpc/CloudGRPC.hpp
    ${CMAKE_SOURCE_DIR}/api-cpp-grpc/include/dfx/api/grpc/DeviceGRPC.hpp
    ${CMAKE_SOURCE_DIR}/api-cpp-grpc/include/dfx/api/grpc/MeasurementGRPC.hpp
    ${CMAKE_SOURCE_DIR}/api-cpp-grpc/include/dfx/api/grpc/MeasurementStreamGRPC.hpp
    ${CMAKE_SOURCE_DIR}/api-cpp-grpc/include/dfx/api/grpc/SignalGRPC.hpp
    ${CMAKE_SOURCE_DIR}/api-cpp-grpc/include/dfx/api/grpc/StudyGRPC.hpp
    ${CMAKE_SOURCE_DIR}/api-cpp-grpc/include/dfx/api/grpc/UserGRPC.hpp)

add_library(
  api-cpp-grpc OBJECT
  $<TARGET_OBJECTS:api-protos-grpc>
  src/CloudGRPCMacros.hpp
  src/CloudGRPC.cpp
  src/DeviceGRPC.cpp
  src/MeasurementGRPC.cpp
  src/MeasurementStreamGRPC.cpp
  src/SignalGRPC.cpp
  src/StudyGRPC.cpp
  src/UserGRPC.cpp)

if(MSVC)
  # Silence the Microsoft compiler warnings C4996/STL4015, for generated gRPC code when building with C++17
  if(CMAKE_CXX_STANDARD GREATER_EQUAL 17)
    target_compile_definitions(api-cpp-grpc PUBLIC "-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING")
  endif()
  if(BUILD_SHARED_LIBS)
    # Warning C4251 complaining about std::string on a DLL boundary
    target_compile_options(api-cpp-grpc PUBLIC /wd4251)
  endif()
  target_compile_options(api-cpp-grpc PUBLIC /utf-8)
endif()

# set_target_properties(api-cpp-grpc PROPERTIES DEFINE_SYMBOL "dfxapi_EXPORTS")
target_compile_definitions(api-cpp-grpc PRIVATE -Ddfxapi_EXPORTS)

target_include_directories(
  api-cpp-grpc
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-cpp,INTERFACE_INCLUDE_DIRECTORIES>>
          $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-protos-grpc,INTERFACE_INCLUDE_DIRECTORIES>>)

set_target_properties(api-cpp-grpc PROPERTIES PUBLIC_HEADER "${API_CPP_GRPC_PUBLIC_HEADERS}")

target_link_libraries(
  api-cpp-grpc protobuf::protobuf gRPC::grpc++ absl::random_internal_randen_hwaes_impl # implicit gRPC
                                                                                       # libgrpc.a
  absl::random_internal_pool_urbg # implicit gRPC libgrpc.a
)

install(TARGETS api-cpp-grpc PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dfx/api/grpc)
