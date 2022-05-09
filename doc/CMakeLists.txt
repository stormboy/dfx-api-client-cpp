if(NOT WITH_DOCS)
  return() # WITH_DOCS is only supported if built WITH_DOCS
endif(NOT WITH_DOCS)

message(STATUS "Building documentation")

find_package(Doxygen)
if(NOT Doxygen_FOUND)
  message(FATAL_ERROR "Failed to locate Doxygen executable required for WITH_DOCS option")
else()
  message(STATUS "  Using Doxygen: ${DOXYGEN_EXECUTABLE}")
endif()

if(DOC_LANGUAGE STREQUAL "chinese")
  set(DOXYGEN_OUTPUT_LANGUAGE "Chinese")
else() # Default is English
  set(DOXYGEN_OUTPUT_LANGUAGE "English")
  set(PDF_OUTPUT_LANGUAGE "en_US")
endif()

add_subdirectory(api-cpp-doxygen)
add_subdirectory(api-protos-grpc-doxygen)
add_subdirectory(api-protos-web-doxygen)

find_package(Python COMPONENTS Interpreter)

if(NOT Python_FOUND)
  message(FATAL_ERROR "Failed to locate Python executable required for WITH_DOCS option")
else()
  message(STATUS "  Using Python: ${Python_EXECUTABLE}")
endif()

# Sphinx should be in the same bin location as Python, i.e. your running in a virtual environment with a pip
# installed sphinx and not trying to leverage an independent sphinx-doc package.
find_package(Sphinx)
if(NOT SPHINX_FOUND)
  message(FATAL_ERROR "Failed to locate Sphinx (sphinx-build) tool required for WITH_DOCS option")
else()
  message(STATUS "  Using Sphinx: ${SPHINX_EXECUTABLE}")
endif()

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/sphinx)
set(SPHINX_BUILD_HTML ${CMAKE_CURRENT_BINARY_DIR}/sphinx/html)
set(SPHINX_BUILD_PDF ${CMAKE_CURRENT_BINARY_DIR}/sphinx/pdf)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}) # folders are named like api-cpp-doxygen

add_custom_target(
  SphinxHTML ALL
  COMMAND
    ${CMAKE_COMMAND} -E env "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}"
    # Sphinx Executable sphinx-build is a Python script, but would use it's own virtual environment and we
    # need to let our virtual environment containing sphinx_rtd_theme, rst2pdf etc. take precedence.
    ${Python_EXECUTABLE} ${SPHINX_EXECUTABLE} -b html
    # Tell Breathe where to find the Doxygen output
    -Dbreathe_projects.CloudAPI=${DOXYGEN_OUTPUT_DIR}/api-cpp-doxygen/xml
    -Dbreathe_projects.ProtosGRPC=${DOXYGEN_OUTPUT_DIR}/api-protos-grpc-doxygen/xml
    -Dbreathe_projects.ProtosWeb=${DOXYGEN_OUTPUT_DIR}/api-protos-web-doxygen/xml ${SPHINX_SOURCE}
    ${SPHINX_BUILD_HTML}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating HTML documentation with Sphinx ${SPHINX_EXECUTABLE}"
  DEPENDS doxygen-api-cpp doxygen-protos-grpc doxygen-protos-web)

# Style examples https://rst2pdf.org/examples.html Passing -a forces a rebuild of all, which avoids a rst2pdf
# problem with searching for updated files because it hasn't been updated in the past year while sphinx has
# had ongoing updates. There might be other issues but the only one that was causing problems happened during
# re-builds while clean builds and forced all builds works without crashing. There isn't enough rst files
# being processed not to just process all. add_custom_target( SphinxPDF ALL COMMAND ${CMAKE_COMMAND} -E env
# "pdf_use_coverpage=True" # useful for testing this works "pdf_language=${PDF_OUTPUT_LANGUAGE}"
# "pdf_style_path=${CMAKE_CURRENT_SOURCE_DIR}/rst2pdf" "pdf_stylesheets=sphinx,custom_styles.txt"
# ${Python_EXECUTABLE} ${SPHINX_EXECUTABLE} -a -b pdf # Tell Breathe where to find the Doxygen output
# -Dbreathe_projects.CloudAPI=${DOXYGEN_OUTPUT_DIR}/api-cpp-doxygen/xml
# -Dbreathe_projects.ProtosGRPC=${DOXYGEN_OUTPUT_DIR}/api-protos-grpc-doxygen/xml
# -Dbreathe_projects.ProtosWeb=${DOXYGEN_OUTPUT_DIR}/api-protos-web-doxygen/xml ${SPHINX_SOURCE}
# ${SPHINX_BUILD} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating documentation with
# rst2pdf" DEPENDS doxygen-api-cpp doxygen-protos-grpc doxygen-protos-web)

add_custom_target(
  SphinxLatex ALL
  COMMAND
    ${CMAKE_COMMAND} -E env "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}"
    # Sphinx Executable sphinx-build is a Python script, but would use it's own virtual environment and we
    # need to let our virtual environment containing sphinx_rtd_theme, rst2pdf etc. take precedence.
    ${Python_EXECUTABLE} ${SPHINX_EXECUTABLE} -b latex
    # Tell Breathe where to find the Doxygen output
    -Dbreathe_projects.CloudAPI=${DOXYGEN_OUTPUT_DIR}/api-cpp-doxygen/xml
    -Dbreathe_projects.ProtosGRPC=${DOXYGEN_OUTPUT_DIR}/api-protos-grpc-doxygen/xml
    -Dbreathe_projects.ProtosWeb=${DOXYGEN_OUTPUT_DIR}/api-protos-web-doxygen/xml ${SPHINX_SOURCE}
    ${SPHINX_BUILD_PDF}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating Latex documentation with Sphinx ${SPHINX_EXECUTABLE}"
  DEPENDS doxygen-api-cpp doxygen-protos-grpc doxygen-protos-web)

add_custom_command(
  TARGET SphinxLatex
  POST_BUILD
  COMMAND make
  WORKING_DIRECTORY ${SPHINX_BUILD_PDF}
  COMMENT "Running Make PDF")
