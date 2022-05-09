# Regularly, I would not use GLOB to specify dependencies but since this is being imported and thus is
# non-editable it won't change and should be safe.
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_BINARY_DIR}/protos-web/*/*.proto)

set(DOXYGEN_PROJECT_NAME ProtosWeb)
set(DOXYGEN_INPUT_DIR ${CMAKE_BINARY_DIR}/protos-web/)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYFILE_IN Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# These are injected via conan package protomatlabdoxygen which establishes the CMAKE_PROGRAM_PATH which
# find_program uses to locate the binary. They are then substituted in the configure_file.
find_program(PROTO2CPP_BINPATH proto2cpp.py)
find_program(M2CPP_BINPATH m2cpp.pl)

configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) # Doxygen won't create this for us
add_custom_command(
  OUTPUT ${DOXYGEN_INDEX_FILE}
  DEPENDS ${SOURCE_FILES}
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
  MAIN_DEPENDENCY ${DOXYFILE_OUT}
  ${DOXYFILE_IN}
  COMMENT "Generating ${DOXYGEN_PROJECT_NAME} docs")

add_custom_target(doxygen-protos-web ALL DEPENDS ${DOXYGEN_INDEX_FILE})
