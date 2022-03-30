if(NOT WITH_VALIDATORS)
  add_library(api-cpp-validator OBJECT src/NoValidator.cpp)
  target_include_directories(api-cpp-validator PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
  install(TARGETS api-cpp-validator PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dfx/api/validator)
  return() # VALIDATORS are only supported if built WITH_VALIDATORS
endif(NOT WITH_VALIDATORS)

add_definitions(-DWITH_VALIDATORS)

# Use an absolute reference here so that doxygen can locate in the doc context by target
set(API_CPP_VALIDATOR_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/validator/CloudValidator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/validator/DeviceValidator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/validator/LicenseValidator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/validator/MeasurementValidator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/validator/MeasurementStreamValidator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/validator/OrganizationValidator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/validator/ProfileValidator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/validator/SignalValidator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/validator/StudyValidator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/dfx/api/validator/UserValidator.hpp)

add_library(
  api-cpp-validator OBJECT
  src/CloudValidator.cpp
  src/DeviceValidator.cpp
  src/LicenseValidator.cpp
  src/MeasurementValidator.cpp
  src/MeasurementStreamValidator.cpp
  src/OrganizationValidator.cpp
  src/ProfileValidator.cpp
  src/SignalValidator.cpp
  src/StudyValidator.cpp
  src/UserValidator.cpp
  src/CloudValidatorMacros.hpp
  ${API_CPP_VALIDATOR_HEADERS})

if(MSVC)
  if(BUILD_SHARED_LIBS)
    # Warning C4251 complaining about std::string on a DLL boundary
    target_compile_options(api-cpp-validator PUBLIC /wd4251)
  endif()
  target_compile_options(api-cpp-validator PUBLIC /utf-8)
endif()

set_target_properties(api-cpp-validator PROPERTIES DEFINE_SYMBOL "dfxcloud_EXPORTS")

target_include_directories(
  api-cpp-validator
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src> # for ValidatorMacros.hpp
          $<BUILD_INTERFACE:$<TARGET_PROPERTY:api-cpp,INTERFACE_INCLUDE_DIRECTORIES>>
          $<BUILD_INTERFACE:$<TARGET_PROPERTY:fmt::fmt,INTERFACE_INCLUDE_DIRECTORIES>>)

set_target_properties(api-cpp-validator PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(api-cpp-validator PROPERTIES CMAKE_VISIBILITY_INLINES_HIDDEN YES)
