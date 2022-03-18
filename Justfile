# just manual: https://github.com/casey/just/#readme

set shell := ["bash", "-uc"]
set positional-arguments

export BUILD_FOLDER:="build"
export t:="Debug"     # Assume type Debug
export c:="False"     # Assume clang-checks off

# The tests below for MSYSTEM are to detect if running in git bash for Windows
# which need to have the paths rewritten to handle Unix style

_default:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ -z "MSYSTEM" ]; then
      {{just_executable()}} -f {{justfile()}} --list --unsorted
    else
      justfile_exe=`cygpath "{{just_executable()}}"`
      justfile_path=`cygpath "{{justfile()}}"`
      "${justfile_exe}" -f "${justfile_path}" --list --unsorted
    fi

# Build using Conan and build folder
build:
    #!/usr/bin/env bash
    set -euo pipefail
    rm -rf "${BUILD_FOLDER}"
    conan install . --build missing -if "${BUILD_FOLDER}" -o dfxcloud:enable_checks="$c" -s build_type="$t"
    cd build
    if [ -z "${MSYSTEM+x}" ]; then
      cmake .. -DCMAKE_TOOLCHAIN_FILE=conan/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$t
      make -j$(nproc)
    else
      cmake .. -DCMAKE_TOOLCHAIN_FILE=conan/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$t
      cmake --build . --config $t --target ALL_BUILD
    fi

# Clang format files
format:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ -z "${MSYSTEM+x}" ]; then
      cd "${BUILD_FOLDER}"
      make fix-clang-format
    else
      echo "Unsupported on Windows"
    fi

_format-cmake:
    #!/usr/bin/env bash
    set -euo pipefail
    cd "${BUILD_FOLDER}"
    make fix-cmake-format

# Clang tidy to check files
check:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ -z "${MSYSTEM+x}" ]; then
      {{just_executable()}} -f {{justfile()}} c="True" build
    else
      justfile_exe=`cygpath "{{just_executable()}}"`
      justfile_path=`cygpath "{{justfile()}}"`
      "${justfile_exe}" -f "${justfile_path}" c="True" build
    fi

# Build a release build
release:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ -z "${MSYSTEM+x}" ]; then
      {{just_executable()}} -f {{justfile()}} t="Release" build
    else
      justfile_exe=`cygpath "{{just_executable()}}"`
      justfile_path=`cygpath "{{justfile()}}"`
      "${justfile_exe}" -f "${justfile_path}" t="Release" build
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
      build/test/$t/test-cloud-api.exe -context="{{context}}"
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
    rm -rf "${BUILD_FOLDER}"
    conan install . --build missing -if "${BUILD_FOLDER}" -o dfxcloud:with_docs=True -s build_type="$t"
    cd build
    if [ -z "${MSYSTEM+x}" ]; then
      cmake .. -DCMAKE_TOOLCHAIN_FILE=conan/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$t
      make -j$(nproc)
    else
      cmake .. -DCMAKE_TOOLCHAIN_FILE=conan/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$t
      cmake --build . --config $t --target ALL_BUILD
    fi

# Build/test via Conan into Conan package cache
conan:
    #!/usr/bin/env bash
    set -euo pipefail
    conan create . --build missing -o dfxcloud:enable_checks="$c"

# Updates licenses using https://github.com/lsm-dev/license-header-checker
update-license:
    license-header-checker -a -v -r resources/license-header.txt . hpp cpp

# install ubuntu system packages
install-ubuntu-system:
    #!/usr/bin/env bash
    set -euo pipefail
    sudo apt update
    sudo apt install cmake
    sudo apt install clang-format
    sudo apt install clang-tidy

