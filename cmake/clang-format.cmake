# Relies on: https://github.com/TheLartians/Format.cmake https://github.com/cpm-cmake/CPM.cmake

include(cmake/CPM.cmake)

# brew install clang-format   # for clang-format pip install cmakelang       # for cmake-format

# Format.cmake adds three additional targets to your CMake project.
#
# format Shows which files are affected by clang-format check-format Errors if files are affected by
# clang-format (for CI integration) fix-format Applies clang-format to all affected files
cpmaddpackage(
  NAME
  Format.cmake
  VERSION
  1.7.0
  GITHUB_REPOSITORY
  TheLartians/Format.cmake
  OPTIONS
  # set to yes skip cmake formatting
  "FORMAT_SKIP_CMAKE NO"
  # path to exclude (optional, supports regular expressions)
  "CMAKE_FORMAT_EXCLUDE cmake/CPM.cmake")
