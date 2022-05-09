# just manual: https://github.com/casey/just/#readme

set shell := ["bash", "-uc"]
set positional-arguments

# These can be overwritten on command line by specifying before command, ie.:
#
#    just folder=output type=Release build
#
export folder:="build"   # Assume "build" as the build folder
export type:="Debug"     # Assume type Debug
export checks:="False"   # Assume clang-checks off
export shared:="True"    # Assume shared library
export docs:="False"     # Assume doc generation off

# The tests below for MSYSTEM are to detect if running in git bash for Windows
# which need to have the paths rewritten to handle Unix style

_default: (_display "Default settings:")
    #!/usr/bin/env bash
    set -euo pipefail
    if [ -z "${MSYSTEM+x}" ]; then
      {{just_executable()}} -f {{justfile()}} --list --unsorted
    else
      justfile_exe=`cygpath "{{just_executable()}}"`
      justfile_path=`cygpath "{{justfile()}}"`
      "${justfile_exe}" -f "${justfile_path}" --list --unsorted
    fi

# Display the currently used build settings with a message
_display message:
    # {{message}}
    @echo -e "\tBuild Type:      type=\"$type\"\t(Debug, Release)"
    @echo -e "\tBuild Folder:    folder=\"$folder\""
    @echo -e "\tShared/Static:   shared=\"$shared\"\t(True, False)"
    @echo -e "\tEnable Checks:   checks=\"$checks\"\t(True, False)"
    @echo -e "\tEnable Docs:     docs==\"$docs\"\t(True, False)\n"

# Build using Conan and build folder
build: (_display "Building with settings:")
    #!/usr/bin/env bash
    set -euo pipefail
    rm -rf "${folder}"
    conan install . -pr:b=default --build missing -if "${folder}" -o dfxcloud:enable_checks="${checks}" -o dfxcloud:with_docs="${docs}" -o dfxcloud:shared="${shared}" -s build_type="${type}"
    cd "${folder}"
    if [ -z "${MSYSTEM+x}" ]; then
      cmake .. -DCMAKE_TOOLCHAIN_FILE=conan/conan_toolchain.cmake
      make -j$(nproc) install
    else
      cmake .. -DCMAKE_TOOLCHAIN_FILE=conan/conan_toolchain.cmake
      cmake --build . --config "${type}" --target ALL_BUILD
    fi

# Format files
format:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ -z "${MSYSTEM+x}" ]; then
      cd "${folder}"
      make fix-format
    else
      echo "Unsupported on Windows"
    fi

# Clang tidy to check files
check:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ -z "${MSYSTEM+x}" ]; then
      {{just_executable()}} -f {{justfile()}} checks="True" folder="${folder}" type="${type}" shared="${shared}" docs="${docs}" build
    else
      justfile_exe=`cygpath "{{just_executable()}}"`
      justfile_path=`cygpath "{{justfile()}}"`
      "${justfile_exe}" -f "${justfile_path}" checks="True" folder="${folder}" type="${type}" shared="${shared}" docs="${docs}" build
    fi

# Build a release build
release:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ -z "${MSYSTEM+x}" ]; then
      {{just_executable()}} -f {{justfile()}} checks="${checks}" folder="${folder}" type="Release" shared="${shared}" docs="${docs}" build
    else
      justfile_exe=`cygpath "{{just_executable()}}"`
      justfile_path=`cygpath "{{justfile()}}"`
      "${justfile_exe}" -f "${justfile_path}" checks="${checks}" folder="${folder}" type="Release" shared="${shared}" docs="${docs}" build build
    fi

# Test (using gtest client)
test context="":
    #!/usr/bin/env bash
    set -euo pipefail
    if [ -z "${MSYSTEM+x}" ]; then
      build/test/test-cloud-api -context="{{context}}"
    else
      justfile_exe=`cygpath "{{just_executable()}}"`
      justfile_path=`cygpath "{{justfile()}}"`
      build/test/${type}/test-cloud-api.exe -context="{{context}}"
    fi

docs:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ -z "${VIRTUAL_ENV+x}" ]; then
      echo "docs target expects a Python virtual environment with requirements.txt installed."
      echo "ie."
      echo "   python3 -m venv venv"
      echo "   source venv/bin/activate"
      echo "   pip install -r requirements.txt"
      exit 1
    fi
    if [ -z "${MSYSTEM+x}" ]; then
      {{just_executable()}} -f {{justfile()}} checks="${checks}" folder="${folder}" type="${type}" shared="${shared}" docs="True" build
    else
      justfile_exe=`cygpath "{{just_executable()}}"`
      justfile_path=`cygpath "{{justfile()}}"`
      "${justfile_exe}" -f "${justfile_path}" checks="${checks}" folder="${folder}" type="${type}" shared="${shared}" docs="True" build build
    fi

# Build/test via Conan into Conan package cache
conan:
    #!/usr/bin/env bash
    set -euo pipefail
    conan create . -pr:b=default --build missing -if "${folder}" -o dfxcloud:enable_checks="${checks}" -o dfxcloud:with_docs="${docs}" -o dfxcloud:shared="${shared}" -s build_type="${type}"

# Updates licenses using https://github.com/lsm-dev/license-header-checker
update-license:
    license-header-checker -a -v -r resources/license-header.txt . hpp cpp

# Current version of library
version:
    #!/usr/bin/env bash
    set -euo pipefail
    grep "set.*_LIBRARY_VERSION" CMakeLists.txt | cut -d' ' -f2 | cut -d')' -f1

# install ubuntu system packages
install-ubuntu-system:
    #!/usr/bin/env bash
    set -euo pipefail
    sudo apt update
    sudo apt install cmake
    sudo apt install clang-format
    sudo apt install clang-tidy
