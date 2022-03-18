if(NOT WITH_TESTS)
  return()
endif()

# https://grpc.github.io/grpc/cpp/md_doc_unit_testing.html

add_executable(
  test-cloud-api
  src/CloudTests.cpp
  src/DeviceTests.cpp
  src/LicenseTests.cpp
  src/MeasurementTests.cpp
  src/OrganizationTests.cpp
  src/ProfileTests.cpp
  src/SignalTests.cpp
  src/StudyTests.cpp
  src/UserTests.cpp)

if(MSVC) # Microsoft compiler flag
  # LNK4099 PDB 'filename' was not found with 'object/library' or at 'path'; linking object as if no debug
  # info
  target_compile_options(test-cloud-api PUBLIC /wd4099)
  # Warning C4251 complaining about std::string on a DLL boundary
  target_compile_options(test-cloud-api PUBLIC /wd4251)
  set_target_properties(test-cloud-api PROPERTIES LINK_FLAGS "/ignore:4099")
endif(MSVC)

# gtest is linked with visibility default and generates warnings if we link with hidden. Since this is only a
# test program, it is fine to link with with default visibility Bug:
# https://bugs.webkit.org/show_bug.cgi?id=224812 https://gist.github.com/ax3l/ba17f4bb1edb5885a6bd01f58de4d542
# set_target_properties(test-cloud-api PROPERTIES CXX_VISIBILITY_PRESET default)
# set_target_properties(test-cloud-api PROPERTIES CMAKE_VISIBILITY_INLINES_HIDDEN NO)
# target_compile_definitions(test-cloud-api PUBLIC GTEST_API_=)
target_link_libraries(test-cloud-api PRIVATE dfxapi gtest::gtest gflags::gflags)

target_include_directories(test-cloud-api PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(APPLE)
  set_target_properties(test-cloud-api PROPERTIES INSTALL_RPATH "@executable_path/../lib")
elseif(WIN32)
  # NO @RPATH on Windows gtest_discover_tests runs in the build folder prior to install, so copy the DLL
  # beside the executable in order to resolve.
  add_custom_command(
    TARGET test-cloud-api
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dfxapi> $<TARGET_FILE_DIR:test-cloud-api>)
else()
  set_target_properties(test-cloud-api PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
endif()

install(
  TARGETS test-cloud-api
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}) # This is for Windows

gtest_discover_tests(test-cloud-api)
