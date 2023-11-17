# just manual: https://github.com/casey/just/#readme

set shell := ['nu', '-c']
set positional-arguments
set dotenv-load
set export

# These can be overwritten on command line by specifying before command, ie.:
#
#    just folder=output type=Release build
#
export checks                    := env_var_or_default("checks", "False")           # Assume clang-checks off
export shared                    := env_var_or_default("shared", "True")            # Assume shared library
export docs                      := env_var_or_default("docs", "False")             # Assume doc generation off
export measurement_only          := env_var_or_default("measurement_only", "False") # Assume measurement_only off
export with_curl                 := env_var_or_default("with_curl", "True")         # Assume with_curl on
export with_rest                 := env_var_or_default("with_rest", "True")         # Assume with_rest on
export with_websocket_json       := env_var_or_default("with_websocket_json", "True")      # Assume with_websocket_json on
export with_websocket_protobuf   := env_var_or_default("with_websocket_protobuf", "False") # Assume with_websocket_proto off
export with_grpc                 := env_var_or_default("with_grpc", "False")         # Assume with_grpc off
export with_validators           := env_var_or_default("with_validators", "True")   # Assume with_validators on
export with_dfxcli               := env_var_or_default("with_dfxcli", "True")       # Assume with_dfxcli on
export with_yaml                 := env_var_or_default("with_yaml", "True")         # Assume with_yaml on

export build                     := env_var_or_default("build", "missing")
export build_type                := env_var_or_default("build_type", "Release")
export only_test                 := env_var_or_default("only_test", "False")
export show_cli                  := env_var_or_default("show_cli", "False")
export test_folder               := env_var_or_default("test_folder", "''")

export folder                    := env_var_or_default("folder", "build")           # Assume "build" as the build folder

# Use "nura" as your conan remote name, or update the .env to match your setting.
export conan_remote              := env_var_or_default("conan_remote", "nura")

# Apple M1 and Windows ARM, the target would be armv8 while most others would be x86_64.
# You can check your default by running "just show" and looking at arch. If it is "wrong"
# it is because your executable is running in emulation mode in which case you can
# get a native build, or you can change it with one of the following:
#
# Set an environment variable "target_arch=armv8"
#
# Add the line below to the .env file in the same folder as Justfile.
# target_arch=armv8
#
# Alternatively, you could prefix the just command as in,
#    target_arch=armv8 just release
export target_arch               := env_var_or_default("target_arch", 'default')         # Used by release / debug targets

_default: (_display "Default settings:")
    @{{just_executable()}} -f {{justfile()}} --list --unsorted

# Display the currently used build settings with a message
_display message:
    # {{message}}
    @print $"\tBuild Type:      type=\"($env.build_type)\"\t\(Debug, Release\)"
    @print $"\tShared/Static:   shared=\"($env.shared)\"\t\(True, False\)"
    @print $"\tEnable Checks:   checks=\"($env.checks)\"\t\(True, False\)"
    @print $"\tEnable Docs:     docs==\"($env.docs)\"\t\(True, False\)\n"

# Build everything possible on platform
all: export macos ios android emscripten windows linux

# Build debug for target platform and archictecture
@debug:
    {{just_executable()}} -f {{justfile()}} build_type=Debug build "{{os()}}" $env.target_arch

# Build a release for target platform and architecture
@release:
    {{just_executable()}} -f {{justfile()}} build_type=Release build "{{os()}}" $env.target_arch

# Run conan test
@_conan_test test="test_package":
    {{just_executable()}} -f {{justfile()}} ("build_type=" + $env.build_type) test_folder={{test}} only_test=True _do_build "{{os()}}" $env.target_arch

# Build conan package for a specific operating system and architecture
@build os=os() arch=target_arch *CONAN_FLAGS="":
    {{just_executable()}} -f {{justfile()}} ("build_type=" + $env.build_type) _do_build "{{os}}" "{{arch}}"

# Performs the build or test
@_do_build os target_arch *CONAN_FLAGS: (_display "Building with settings:")
    let target_arch = (if ("{{target_arch}}" == "default") { if ("{{arch()}}" == "aarch64") { "armv8" } else { "{{arch()}}" } } else { "{{target_arch}}" }); \
    if ( ("{{os()}}" == "macos"   and ("{{os}}" == "macos"   or "{{os}}" == "android" or "{{os}}" == "emscripten" or "{{os}}" == "ios")) or \
          ("{{os()}}" == "linux"   and ("{{os}}" == "linux"   or "{{os}}" == "android" or "{{os}}" == "emscripten")) or \
          ("{{os()}}" == "windows" and ("{{os}}" == "windows" or "{{os}}" == "android" or "{{os}}" == "emscripten")) ) { \
        $env.command_line = (["-pr:b", (('{{justfile_directory()}}' | str replace '\' '/' -a) + '/conan/profiles/{{os}}/build-default'), \
                           "-pr:h", (('{{justfile_directory()}}' | str replace '\' '/' -a) + '/conan/profiles/{{os}}/' + $target_arch), \
                           "--build", $env.build, \
                           "-o", ("dfxcloud/*:enable_checks=" + $env.checks), \
                           "-o", ("dfxcloud/*:shared=" + $env.shared), \
                           "-o", ("dfxcloud/*:docs=" + $env.docs), \
                           "-o", ("dfxcloud/*:measurement_only=" + $env.measurement_only), \
                           "-o", ("dfxcloud/*:with_curl=" + $env.with_curl), \
                           "-o", ("dfxcloud/*:with_rest=" + $env.with_rest), \
                           "-o", ("dfxcloud/*:with_websocket_json=" + $env.with_websocket_json), \
                           "-o", ("dfxcloud/*:with_websocket_protobuf=" + $env.with_websocket_protobuf), \
                           "-o", ("dfxcloud/*:with_grpc=" + $env.with_grpc), \
                           "-o", ("dfxcloud/*:with_validators=" + $env.with_validators), \
                           "-o", ("dfxcloud/*:with_dfxcli=" + $env.with_dfxcli), \
                           "-o", ("dfxcloud/*:with_yaml=" + $env.with_yaml), \
                           "-s", ("build_type=" + $env.build_type)]); \
        if ( ("{{CONAN_FLAGS}}" | str length) > 0 ) { $env.command_line = ($env.command_line | append ( "{{CONAN_FLAGS}}" | split words)); }; \
        if ( $env.only_test == True ) { \
            let reference = (conan inspect . --format json | from json | get name version | reduce { |it, ac| $"( $ac + '/' + $it)" }); \
            let command_line = (($env.command_line | prepend [ "conan", "test" ] | append [$env.test_folder, $reference]) | reduce {|it,acc| $"($acc + ' ' + $it)"}); \
            print ($"TEST FOR: {{os()}} \"{{os}}/($target_arch)\""); \
            print $command_line; \
            nu -c $command_line; \
        } else if ( $env.show_cli == True ) { \
            print ($env.command_line | to nuon); \
        } else { \
            let command_line = (($env.command_line | prepend [ "conan", "create", ".", "-tf", $env.test_folder]) | reduce {|it,acc| $"($acc + ' ' + $it)"}); \
            print ($"BUILD FOR: {{os()}} \"{{os}}/($target_arch)\""); \
            print $command_line; \
            nu -c $command_line; \
        }; \
    } else { \
        echo "Error: {{os()}} does not support building for {{os}}/{{target_arch}}" \
    }

# Build for all Mac platforms (armv8, x86_64)
macos: (build "macos" "armv8") (build "macos" "x86_64")

# Build for all Windows platforms (armv8, x86_64)
windows: (build "windows" "x86_64") (build "windows" "armv8")

# Build for all Linux platforms (armv8, x86_64)
linux: (build "linux" "x86_64") (build "linux" "armv8")

# Build for all Android platforms (armv8, armv7, x86, x86_64)
android: (build "android" "armv8") (build "android" "armv7") (build "android" "x86") (build "android" "x86_64")

# Build for all iOS platforms (armv8, simulator64, simulatorarm64)
ios: (build "ios" "armv8") (build "ios" "simulator64") (build "ios" "simulatorarm64")

# Build for Emscripten
emscripten: (build "emscripten" "emscripten")

# Remove all the conan recipes. Usually sufficient to just export.
@remove recipe='.*':
    "Removing '{{recipe}}'. This may take a minute." | print; \
    open dependencies.yaml | transpose id fields | each { |it| if ($it.id =~ "{{recipe}}") { \
        nu -c ("conan remove -c " + $it.fields.name + "/" + $it.fields.version); } }; "";

# Export all the conan recipes into local cache
@export recipe='.*':
    open dependencies.yaml | transpose id fields | each { |it| if ($it.id =~ "{{recipe}}") { \
        nu -c ("conan export " + $it.fields.recipe + " --version " + $it.fields.version); } }; "";

# Upload dependent packages to Conan remote
@upload recipe="*" *UPLOAD_OPTS="--only-recipe":
    open dependencies.yaml | transpose id fields | each { |it| if ($it.id =~ "{{recipe}}") { \
        nu -c ("conan upload " + $it.fields.name + "/" + $it.fields.version + " -r " + $env.conan_remote + " {{UPLOAD_OPTS}}"); } }; "";

# List the dependency recipes and versions
@list recipe='.*':
    open dependencies.yaml | transpose id fields | sort-by id | \
        each { |it| if ($it.id =~ "{{recipe}}") { $it.fields | select recipe name version } };

@_show_cli os=os() arch=target_arch:
    {{just_executable()}} -f {{justfile()}} ("build_type=" + $env.build_type) show_cli=True _do_build "{{os}}" "{{arch}}"

# List the latest conan package details in your cache
@latest:
    let reference = (conan inspect . --format json | from json | get name version | reduce { |it, ac| $"( $ac + '/' + $it + '#latest:*')" }); \
    print ("conan list " + $reference); \
    conan list $reference

# Show the platform information
@show:
    ("date:   \"" + (date now | format date "%Y-%m-%d %H:%M:%S") + "\"") | print; \
    ("host:   \"" + (sys | get host.name) + " " + (sys | get host.os_version) + "\"") | print; \
    ("os:     {{os()}}") | print; \
    ("arch:   {{arch()}}") | print; \
    ("conan:  " + (conan --version | str substring 14..)) | print; \
    ("just:   " + (just --version | str substring 5..)) | print; \
    ("nu:     " + (nu --version)) | print; \
    ("-------------------------") | print; \
    open --raw (conan profile path default)


# Performs the build or test
@_do_cmake os target_arch *CONAN_FLAGS: (_display "Building with settings:")
    $env.command_line = (["-B", $env.folder, \
                       "-DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake", \
                       ("-DCMAKE_BUILD_TYPE=" + ($env.build_type | str capitalize)), \
                       $"-DENABLE_CHECKS=($env.checks)", \
                       $"-DBUILD_SHARED_LIBS=($env.shared)", \
                       $"-DWITH_DOCS=($env.docs)", \
                       $"-DMEASUREMENT_ONLY=($env.measurement_only)", \
                       $"-DWITH_CURL=($env.with_curl)", \
                       $"-DWITH_REST=($env.with_rest)", \
                       $"-DWITH_WEBSOCKET_JSON=($env.with_websocket_json)", \
                       $"-DWITH_WEBSOCKET_PROTOBUF=($env.with_websocket_protobuf)", \
                       $"-DWITH_GRPC=($env.with_grpc)", \
                       $"-DWITH_VALIDATORS=($env.with_validators)", \
                       $"-DWITH_DFXCLI=($env.with_dfxcli)", \
                       $"-DWITH_YAML=($env.with_yaml)"]); \
    if ( $env.cmake_config == True ) { \
        let cmake_config = (($env.command_line | prepend [ "cmake", "-S", ('{{justfile_directory()}}' | str replace '\' '/' -a)]) | reduce {|it,acc| $"($acc + ' ' + $it)"}); \
        print $cmake_config; \
    }

# CMake configure build folder
@config:
    let cmake_configure = (["cmake", "-S", ('{{justfile_directory()}}' | str replace '\' '/' -a), "-B", ([('{{justfile_directory()}}' | str replace '\' '/' -a), $env.folder] | path join), \
                       ("-DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=" + ([('{{justfile_directory()}}' | str replace '\' '/' -a), "conan_provider.cmake"] | path join)), \
                       ("-DCMAKE_BUILD_TYPE=" + ($env.build_type | str capitalize)), \
                       $"-DENABLE_CHECKS=($env.checks)", \
                       $"-DBUILD_SHARED_LIBS=($env.shared)", \
                       $"-DWITH_DOCS=($env.docs)", \
                       $"-DMEASUREMENT_ONLY=($env.measurement_only)", \
                       $"-DWITH_CURL=($env.with_curl)", \
                       $"-DWITH_REST=($env.with_rest)", \
                       $"-DWITH_WEBSOCKET_JSON=($env.with_websocket_json)", \
                       $"-DWITH_WEBSOCKET_PROTOBUF=($env.with_websocket_protobuf)", \
                       $"-DWITH_GRPC=($env.with_grpc)", \
                       $"-DWITH_VALIDATORS=($env.with_validators)", \
                       $"-DWITH_DFXCLI=($env.with_dfxcli)", \
                       $"-DWITH_YAML=($env.with_yaml)"] | reduce {|it,acc| $"($acc + ' ' + $it)"}); \
    print $cmake_configure; \
    nu -c $cmake_configure;

# CMake build the configured project in build folder
@make target="all":
    let num_cpu = ([(( sys | get cpu | length ) - 1), 4] | math max | into string); \
    let build_folder = ([('{{justfile_directory()}}' | str replace '\' '/' -a), $env.folder] | path join); \
    if not ( $build_folder | path exists ) { \
       print $"Unable to locate build folder '($build_folder)'.\nHave you run 'just config'?"; \
       exit 1; \
    } else { \
        let cmake_build = (["cmake", "--build", $build_folder, "--target", {{target}}, "-j", $num_cpu] | reduce {|it,acc| $"($acc + ' ' + $it)"}); \
        print $cmake_build; \
        nu -c $cmake_build; \
    }

# CMake clean target
@clean: (make "clean")

# CMake Format files
format target="fix-format": (make target)

# CMake list targets
@targets: (make "help")

# CMake Test (using gtest client)
@test context="''": (make "test-cloud-api")
    let test_executable = ([ ([('{{justfile_directory()}}' | str replace '\' '/' -a), $env.folder, "test", "test-cloud-api"] | path join), \
                            ([('{{justfile_directory()}}' | str replace '\' '/' -a), $env.folder, "test", "Debug", "test-cloud-api.exe"] | path join), \
                            ('{{justfile_directory()}}' | str replace '\' '/' -a) ] | where { |it| $it | path exists } | first); \
    if $test_executable == ('{{justfile_directory()}}' | str replace '\' '/' -a) { \
       print "Unable to locate 'test-cloud-api', have you run 'just make'?"; \
    } else { \
       nu -c $test_executable "--config={{context}}"; \
    }

# CMake build the PDF document
@docs:
    if ($env | select VIRTUAL_ENV -i | get VIRTUAL_ENV | is-empty) { \
      print "docs target expects a Python virtual environment with requirements.txt installed."; \
      print "ie."; \
      print "   python3 -m venv venv"; \
      print "   source venv/bin/activate"; \
      print "   pip install -r requirements.txt"; \
      exit 1; \
    } else { \
      {{just_executable()}} -f {{justfile()}} docs=True config; \
      {{just_executable()}} -f {{justfile()}} make SphinxLatex; \
      print $"\nDocument Built:\n\n\topen ($env.folder)/doc/sphinx/pdf/DFXCloud.pdf\n\n"; \
    }

# Updates licenses using https://github.com/lsm-dev/license-header-checker
update-license:
    license-header-checker -a -v -r ('{{justfile_directory()}}' | str replace '\' '/' -a) + '/resources/license-header.txt' ('{{justfile_directory()}}' | str replace '\' '/' -a) hpp cpp

# Current version of library
@version:
    conan inspect ('{{justfile_directory()}}' | str replace '\' '/' -a) | from yaml | get version

# Show the options available with conan for package
@options:
    conan inspect ('{{justfile_directory()}}' | str replace '\' '/' -a) | from yaml | get options  | transpose key val | each { |e| print $"($e.key)=($e.val)" } | ignore

# install ubuntu system packages
_install-ubuntu-system:
    sudo apt update
    sudo apt install cmake
    sudo apt install clang-format
    sudo apt install clang-tidy
