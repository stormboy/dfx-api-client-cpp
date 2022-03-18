# -*- coding: utf-8 -*-

from conans import ConanFile, MSBuild, tools
from conans.tools import os_info, SystemPackageTool
from conan.tools.cmake import CMakeToolchain, CMake, CMakeDeps

import os, sys, subprocess

# https://stackoverflow.com/a/42580137
def get_base_prefix_compat():
    return getattr(sys, "base_prefix", None) or getattr(sys, "real_prefix", None) or sys.prefix

def in_virtualenv():
    return get_base_prefix_compat() != sys.prefix

class dfxcloud(ConanFile):
    name = "dfxcloud"
    version = "1.0.0"
    license = "Nuralogix License"
    url = "https://github.com/nuralogix/dfx-api-client-cpp"
    description = "The DFX API facilitates data communication with a DFX Server"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain", "versioninfo"

    exports = "requirements.txt"
    keep_imports = True   # Do not delete between build and package
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "measurement_only": [True, False],
        "with_c_abi": [True, False],
        "with_curl": [True, False],
        "with_grpc": [True, False],
        "with_websocket": [True, False],
        "with_rest": [True, False],
        "with_validators": [True, False],
        "with_yaml": [True, False],
        "with_docs": [True, False],
        "doc_language": ["english", "chinese"],
        "with_tests": [True, False],
        "with_dfxcli": [True, False],
        "with_clang_format": [True, False],
        "enable_checks": [True, False]
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "measurement_only": False,
        "with_c_abi": True,
        "with_curl": True,
        "with_grpc": True,
        "with_websocket": True,
        "with_rest": True,
        "with_validators": True,
        "with_yaml": True,
        "with_docs": False,
        "doc_language": "english",
        "with_tests": True,
        "with_dfxcli": True,
        "with_clang_format": True,
        "enable_checks": False
    }

    scm = {
        "type": "git",
        "subfolder": name,
        "url": "auto",
        "revision": "auto"
    }

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

        # Curl on Macos has an issue with the default darwinssl when the
        # CURLOPT_SSL_VERIFYHOST=false, the server expect SNI and the disabling
        # of hostname validation causes SNI to also be disabled with darwinssl.
        #
        # See: https://github.com/curl/curl/issues/6347#issuecomment-748526449
        if self.settings.os == "Macos" and self.options.with_curl:
            self.options["libcurl"].with_ssl = "openssl"

        if self.settings.os == "Emscripten":
            self.options.with_curl = False
            self.options.with_grpc = False
            self.options.with_rest = False
            self.options.with_docs = False
            self.options.with_tests = False
            self.options.with_yaml = False

        if tools.cross_building(self.settings):
            self.options.with_docs = False
            self.options.with_tests = False

    def system_requirements(self):

        if os_info.linux_distro == "fedora" or os_info.linux_distro == "centos":
            installer = SystemPackageTool()
            installer.install("perl-CPAN")   # Building openssl on Fedora 34 requires Perl module FindBin

        if self.options.with_docs:
            if not in_virtualenv():
                raise Exception("You need to create a virtual environment to build with_docs=True")

            # Don't know where we are building from, but do know the requirements.txt is beside this conanfile.py
            requirements = os.path.join(os.path.dirname(os.path.realpath(__file__)), "requirements.txt")
            subprocess.check_call([sys.executable, '-m', 'pip', 'install', '-r', requirements])

    def configure(self):
        if not self.options.with_docs:
            del self.options.doc_language

    def source(self):
        self.run("git clone {} {}".format(self.url, self.name))
        self.run("cd {}".format(self.name))
        self.run("git checkout tags/v{} -b v{}-branch".format(self.version, self.version))

    def build_requirements(self):
        # Nuralogix conan repository provided
        self.build_requires("versioninfo/1.0.4")              # Inject library dependency information

        if self.options.with_docs:
            self.build_requires("doxygen/1.9.2")
            self.build_requires("protomatlabdoxygen/20200618_2073e51")

        self.build_requires("protobuf/3.19.2")                # Need protoc on build platform
        if self.options.with_grpc:
            self.build_requires("grpc/1.44.0")                # Need grpc_cpp_plugin on build platform

    def requirements(self):
        # The output library strips all shared library symbols for items not in our public
        # interface, thus all these dependencies are considered private implementation details.
        # It leaves it open for package consumers to define their own versions without symbol
        # conflicts while importing this package.

        if self.options.with_tests:
            self.requires("gtest/1.11.0")
            self.requires("gflags/2.2.2")

        if self.options.with_dfxcli:
            self.requires("CLI11/2.1.2")
            self.requires("naturalsort/20210209_5c71f46")

        # Protobuf is a hard requirements to generate sources, not just an override of gRPC
        self.requires("protobuf/3.19.2")

        # openssl is used by cmake, libwebsockets, gRPC, etc. they tend to drift and we want the latest
        # so it needs to be explicitly set here as the latest.
        if self.options.with_grpc or self.options.with_websocket or self.options.with_curl:
            self.requires("openssl/1.1.1m", override=True)               # Override grpc dependent on 1.1.1h

        if self.options.with_grpc:
            self.requires("abseil/20211102.0", override=True)
            self.requires("re2/20220201", override=True)
            self.requires("meson/0.60.2", override=True)
            self.requires("c-ares/1.18.1", override=True)
            self.requires("grpc/1.44.0")

        if self.options.with_curl:
            self.requires("libcurl/7.80.0")                   # Fetch rootca from standalone

        if self.options.with_websocket:
            self.requires("libwebsockets/4.3.0")

        if self.options.with_yaml:
            self.requires("yaml-cpp/0.7.0")                   # Configuration file handling

        self.requires("nlohmann_json/3.10.5")
        self.requires("fmt/8.1.1")

        # Nuralogix Conan repository provided
        self.requires("dfxprotobufspublic/20210805_0094d09")  # gRPC
        self.requires("dfxprotosweb/20211125_4a907b6")        # REST/WebSocket
        self.requires("googleapis/20211130_2198f9f")          # For annotations.proto, http.proto

    def imports(self):
        self.copy(pattern="license*", src="licenses", dst="licenses", folder=True, ignore_case=True)

    def layout(self):
        # Currently, there is no nice way to find out what the install folder is so walk the command
        # line arguments and see if an install folder was passed. When V2 fully lands, this will
        # hopefully be resolved and will be called the build folder. In the transition, this is the
        # best option I've found so far.
        install_folder = None
        for i, arg in enumerate(sys.argv):
            if arg.startswith("-if="):
                install_folder = arg[4:]
            elif arg.startswith("-if"):
                install_folder = sys.argv[i+1]
        if install_folder:
            self.folders.build = install_folder
        else:
            # The default cmake_layout uses "." to identify the source location but when using VS Code
            # the "." is where VS Code was launch from which is problematic. The best way to do this
            # is to anchor relative to this conanfile. The build_type is left as-is instead of
            # dropping to lowercase because VS Code treats it as an environment variable when building
            # paths which would be impossible to match case. This allows the VS Code Conan Plugin to
            # define it's JSON profiles with
            source_path = os.path.dirname(__file__)
            self.folders.build = os.path.join(source_path, "cmake-build-{}".format(self.settings.build_type))

        self.folders.source = self.name   # Because this package uses scm with subfolder, need to match location
        self.folders.generators = os.path.join(self.folders.build, "conan")  # Expect to be relative to build location
        self.folders.imports = self.folders.build   # Want imports to go to present build as they used to

    def generate(self):
        toolchain = CMakeToolchain(self)
        toolchain.variables["BUILD_SHARED_LIBS"] = "True" if self.options.shared else "False"
        toolchain.variables["MEASUREMENT_ONLY"] = "ON" if self.options.measurement_only else "OFF"
        toolchain.variables["WITH_CURL"] = "ON" if self.options.with_curl else "OFF"
        toolchain.variables["WITH_REST"] = "ON" if self.options.with_rest else "OFF"
        toolchain.variables["WITH_WEBSOCKET"] = "ON" if self.options.with_websocket else "OFF"
        toolchain.variables["WITH_GRPC"] = "ON" if self.options.with_grpc else "OFF"
        toolchain.variables["WITH_VALIDATORS"] = "ON" if self.options.with_validators else "OFF"
        toolchain.variables["WITH_YAML"] = "ON" if self.options.with_yaml else "OFF"
        toolchain.variables["WITH_TESTS"] = "ON" if self.options.with_tests else "OFF"
        toolchain.variables["WITH_DFXCLI"] = "ON" if self.options.with_dfxcli else "OFF"
        toolchain.variables["WITH_DOCS"] = "ON" if self.options.with_docs else "OFF"
        toolchain.variables["WITH_CLANG_FORMAT"] = "True" if self.options.with_clang_format else "False"
        toolchain.variables["ENABLE_CHECKS"] = "True" if self.options.enable_checks else "False"
        toolchain.generate()

        # This writes all the config files (xxx-config.cmake)
        cmake = CMakeDeps(self)
        cmake.build_context_activated = ["protobuf"]
        cmake.build_context_suffix = {"protobuf": "Build"}
        cmake.build_context_build_modules = ["protobuf"]
        cmake.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)

        # Generate dfxcloud-config.cmake
        self.cpp_info.set_property("cmake_file_name", "dfxcloud")

        # DFXCloud:: namespace for the targets
        self.cpp_info.set_property("cmake_target_name", "DFXCloud")

        # Generate both DFXCloud-config.cmake and FindDFXCloud.cmake
        self.cpp_info.set_property("cmake_find_mode", "both")

