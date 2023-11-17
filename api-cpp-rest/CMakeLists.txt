if(NOT WITH_REST)
  return() # REST is only supported if built WITH_REST
endif(NOT WITH_REST)

add_definitions(-DWITH_REST)

# Use an absolute reference here so that doxygen can locate in the doc context by target
set(API_CPP_REST_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/rest/DeviceREST.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/rest/LicenseREST.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/rest/MeasurementREST.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/rest/MeasurementStreamREST.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/rest/OrganizationREST.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/rest/ProfileREST.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/rest/SignalREST.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/rest/StudyREST.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/rest/UserREST.hpp)

add_library(
  api-cpp-rest OBJECT
  src/CloudREST.cpp
  src/DeviceREST.cpp
  src/LicenseREST.cpp
  src/MeasurementREST.cpp
  src/MeasurementStreamREST.cpp
  src/OrganizationREST.cpp
  src/ProfileREST.cpp
  src/SignalREST.cpp
  src/StudyREST.cpp
  src/UserREST.cpp
  ${API_CPP_REST_HEADERS})

if(MSVC)
  if(BUILD_SHARED_LIBS)
    # Warning C4251 complaining about std::string on a DLL boundary
    target_compile_options(api-cpp-rest PUBLIC /wd4251)
  endif()
  target_compile_options(api-cpp-rest PUBLIC /utf-8)
endif()

target_compile_definitions(api-cpp-rest PRIVATE -Ddfxcloud_EXPORTS)

target_include_directories(
  api-cpp-rest
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-protos-web,INTERFACE_INCLUDE_DIRECTORIES>>
  PRIVATE $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-cpp,INTERFACE_INCLUDE_DIRECTORIES>>
          $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-cpp-validator,INTERFACE_INCLUDE_DIRECTORIES>>)

set_target_properties(api-cpp-rest PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_link_libraries(
  api-cpp-rest PRIVATE api-protos-web $<$<NOT:$<PLATFORM_ID:Emscripten>>:CURL::libcurl> fmt::fmt
                       nlohmann_json::nlohmann_json openssl::openssl $<$<TARGET_EXISTS:api-utils>:api-utils>)

install(TARGETS api-cpp-rest PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dfx/api/rest)
