# This function generates a header file based on some file content. The way it does it is a bit convoluted but
# roughly similar to this suggestion for embedding shaders in source files
# https://stackoverflow.com/a/11814544.
#
# However, I wanted a CMake function which handled everything and was self contained with the embedfile.c
# source which makes it a bit more convoluted.

function(embed_file symbol input output)
  # Only want to add the embedfile executable once, even though we might call function multiple times
  if(NOT TARGET embedfile)
    add_executable(embedfile "${CMAKE_BINARY_DIR}/generated/embed-file.c")
  endif()

  add_custom_command(
    OUTPUT ${output}
    COMMAND $<TARGET_FILE:embedfile> ${symbol} ${input}
    DEPENDS ${input})
endfunction()

set(_EMBED_FILE_SOURCE
    "
// https://stackoverflow.com/a/11814544

#include <stdlib.h>
#include <stdio.h>

FILE* open_or_exit(const char* fname, const char* mode)
{
  FILE* f = fopen(fname, mode);
  if (f == NULL) {
    perror(fname);
    exit(EXIT_FAILURE);
  }
  return f;
}

int main(int argc, char** argv)
{
  if (argc < 3) {
    fprintf(stderr, \"USAGE: %s {sym} {rsrc}\\n\\n\"
        \"  Creates {sym}.c from the contents of {rsrc}\\n\",
        argv[0]);
    return EXIT_FAILURE;
  }

  const char* sym = argv[1];
  FILE* in = open_or_exit(argv[2], \"r\");

  char symfile[256];
  snprintf(symfile, sizeof(symfile), \"%s.c\", sym);

  FILE* out = open_or_exit(symfile,\"w\");
  fprintf(out, \"#include <stdlib.h>\\n\");
        fprintf(out, \"const char %s[] = {\\n\", sym);

        unsigned char buf[256];
        size_t nread = 0;
        size_t linecount = 0;
        do {
        nread = fread(buf, 1, sizeof(buf), in);
        size_t i;
        for (i=0; i < nread; i++) {
        fprintf(out, \"0x%02x, \", buf[i]);
        if (++linecount == 10) { fprintf(out, \"\\n\"); linecount = 0; }
        }
        } while (nread > 0);
        if (linecount > 0) fprintf(out, \"\\n\");
        fprintf(out, \"};\\n\");
        fprintf(out, \"const size_t %s_len = sizeof(%s);\\n\\n\",sym,sym);

        fclose(in);
        fclose(out);

        return EXIT_SUCCESS;
        }
")

file(WRITE "${CMAKE_BINARY_DIR}/generated/embed-file.c" "${_EMBED_FILE_SOURCE}")
unset(_EMBED_FILE_SOURCE)
