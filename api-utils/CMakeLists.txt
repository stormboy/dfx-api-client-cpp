# add_definitions(-DWITH_VALIDATORS)

# Use an absolute reference here so that doxygen can locate in the doc context by target
set(API_UTILS_PUBLIC_HEADERS ${CMAKE_SOURCE_DIR}/api-utils/include/dfx/api/utils/HexDump.hpp
                             ${CMAKE_SOURCE_DIR}/api-utils/include/dfx/api/utils/FileUtils.hpp)

add_library(api-utils OBJECT src/HexDump.cpp src/FileUtils.cpp)

set_target_properties(api-utils PROPERTIES DEFINE_SYMBOL "dfxapi_EXPORTS")

target_include_directories(
  api-utils PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                   $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>) # For CloudAPI_Export.hpp

target_link_libraries(api-utils PRIVATE fmt::fmt)

set_target_properties(api-utils PROPERTIES PUBLIC_HEADER "${API_UTILS_PUBLIC_HEADERS}")

set_target_properties(api-utils PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(api-utils PROPERTIES CMAKE_VISIBILITY_INLINES_HIDDEN YES)

install(TARGETS api-utils PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dfx/api/utils)
