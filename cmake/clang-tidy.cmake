# option(CMAKE_CXX_CLANG_TIDY "clang-tidy;-style=file;-checks=*") return()

# MacOS: brew install --with-clang --with-clang-extra-tools --with-utils llvm
find_program(
  CLANG_TIDY_EXE
  NAMES "clang-tidy"
  DOC "Path to clang-tidy executable"
  HINTS "/usr/local/opt/llvm/bin" # MacOS brew location
)

if(NOT CLANG_TIDY_EXE)
  message(STATUS "clang-tidy not found.")
else()
  message(STATUS "clang-tidy found: ENABLE [${CLANG_TIDY_EXE}]")
  set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

if(CLANG_TIDY_EXE)

  set_target_properties(api-cpp PROPERTIES CXX_CLANG_TIDY
                                           "${DO_CLANG_TIDY};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")

  set_target_properties(
    api-cpp-grpc PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")

  set_target_properties(
    api-cpp-rest PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")

  set_target_properties(
    api-cpp-websocket PROPERTIES CXX_CLANG_TIDY
                                 "${DO_CLANG_TIDY};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")

  if(WITH_VALIDATORS)
    set_target_properties(
      api-cpp-validator PROPERTIES CXX_CLANG_TIDY
                                   "${DO_CLANG_TIDY};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
  endif(WITH_VALIDATORS)

  set_target_properties(websocket PROPERTIES CXX_CLANG_TIDY
                                             "${DO_CLANG_TIDY};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
endif()
