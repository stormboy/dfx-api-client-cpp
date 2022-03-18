# Look for an executable called sphinx-build
get_filename_component(SPHINX_PATH_HINT ${Python_EXECUTABLE} DIRECTORY)
find_program(
  SPHINX_EXECUTABLE
  HINTS ${SPHINX_PATH_HINT}
  NAMES sphinx-build
  DOC "Path to sphinx-build executable")

include(FindPackageHandleStandardArgs)

# Handle standard arguments to find_package like REQUIRED and QUIET
find_package_handle_standard_args(Sphinx "Failed to find sphinx-build executable" SPHINX_EXECUTABLE)

mark_as_advanced(SPHINX_EXECUTABLE)
