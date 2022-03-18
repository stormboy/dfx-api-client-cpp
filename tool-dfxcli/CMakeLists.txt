if(NOT WITH_DFXCLI)
  return() # Only supported if built WITH_DFXCLI
endif(NOT WITH_DFXCLI)

set(DFXCLI_HEADERS
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/AuthCommands.hpp
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/ConfigCommand.hpp
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/DeviceCommand.hpp
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/DFXAppCommand.hpp
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/LicenseCommand.hpp
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/MeasurementCommand.hpp
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/OrganizationCommand.hpp
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/ProfileCommand.hpp
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/SignalCommand.hpp
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/StatusCommand.hpp
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/StudyCommand.hpp
    ${CMAKE_SOURCE_DIR}/tool-dfxcli/include/dfx/api/cli/UserCommand.hpp)

add_executable(
  dfxcli
  src/MainCLI.cpp
  src/AuthCommands.cpp
  src/ConfigCommand.cpp
  src/DeviceCommand.cpp
  src/DFXAppCommand.cpp
  src/LicenseCommand.cpp
  src/MeasurementCommand.cpp
  src/OrganizationCommand.cpp
  src/ProfileCommand.cpp
  src/SignalCommand.cpp
  src/StatusCommand.cpp
  src/StudyCommand.cpp
  src/UserCommand.cpp
  ${DFXCLI_HEADERS})

target_include_directories(dfxcli PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(dfxcli PRIVATE dfxapi CLI11::CLI11 naturalsort::naturalsort fmt::fmt)

if(APPLE)
  set_target_properties(dfxcli PROPERTIES INSTALL_RPATH "@executable_path/../lib")
elseif(WIN32)
  # NO @RPATH on Windows gtest_discover_tests runs in the build folder prior to install, so copy the DLL
  # beside the executable in order to resolve.
  add_custom_command(
    TARGET dfxcli
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dfxapi> $<TARGET_FILE_DIR:dfxcli>)
else()
  set_target_properties(dfxcli PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
endif()

install(
  TARGETS dfxcli
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}) # This is for Windows
