# -*- coding: utf-8 -*-

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import collect_libs, copy, load
from conan.tools.build import cross_building
from conan.tools.system.package_manager import Yum

import os, sys, re, subprocess
import yaml

# https://stackoverflow.com/a/42580137
def get_base_prefix_compat():
    return getattr(sys, "base_prefix", None) or getattr(sys, "real_prefix", None) or sys.prefix

def in_virtualenv():
    return get_base_prefix_compat() != sys.prefix

def get_version():
    try:
        content = load(os.getcwd(), "CMakeLists.txt")
        version = re.search("set\(DFX_CLOUD_LIBRARY_VERSION (.*)\)", content).group(1)
        return version.strip()
    except Exception as e:
        #print("Exception obtaining version: {}".format(e))
        return None

class dfxcloud(ConanFile):
    name = "dfxcloud"
    version = get_version()
    license = "Nuralogix License"
    url = "https://github.com/nuralogix/dfx-api-client-cpp"
    description = "The DFX API facilitates data communication with a DFX Server"
    settings = "os", "compiler", "build_type", "arch"

#    exports = "requirements.txt", "CMakeLists.txt"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "measurement_only": [True, False],
        "with_c_abi": [True, False],
        "with_curl": [True, False],
        "with_grpc": [True, False],
        "with_websocket_json": [True, False],
        "with_websocket_protobuf": [True, False],
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
        "with_grpc": False,
        "with_websocket_json": True,
        "with_websocket_protobuf": False,
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

    def export(self):
        copy(self, "dependencies.yaml", src=self.recipe_folder, dst=self.export_folder)

    def export_sources(self):
        copy(self, ".*", src=self.recipe_folder, dst=self.export_sources_folder, keep_path=True)  # May need .clang-tidy, etc.
        copy(self, "CMakeLists.txt", src=self.recipe_folder, dst=self.export_sources_folder, keep_path=True)
        for folder in ["api-cpp", "api-cpp-grpc", "api-cpp-rest", "api-cpp-validator", "api-cpp-websocket-json",
                       "api-cpp-websocket-protobuf", "api-protos-grpc", "api-protos-web", "api-utils", "cmake",
                       "doc", "licenses", "resources", "test", "test_data", "tool-dfxcli", "websocket"]:
            copy(self, "*", src=os.path.join(self.recipe_folder, folder), dst=os.path.join(self.export_sources_folder, folder), keep_path=True)

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

        self.options["libwebsockets"].with_zlib="zlib"

        if self.settings.os == "Emscripten":
            self.options.with_curl = False
            self.options.with_grpc = False
            self.options.with_rest = False
            self.options.with_docs = False
            self.options.with_tests = False
            self.options.with_yaml = False

        if cross_building(self):
            self.options.with_docs = False
            self.options.with_tests = False
            self.options.with_dfxcli = False

    def system_requirements(self):
        Yum(self).install(["perl-CPAN"])     # Building openssl on Fedora 34 requires Perl module FindBin

        if self.options.with_docs:
            if not in_virtualenv():
                raise Exception("You need to create a virtual environment to build with_docs=True")

            # Don't know where we are building from, but do know the requirements.txt is beside this conanfile.py
            requirements = os.path.join(os.path.dirname(os.path.realpath(__file__)), "requirements.txt")
            subprocess.check_call([sys.executable, '-m', 'pip', 'install', '-r', requirements])

    def configure(self):
        if not self.options.with_docs:
            del self.options.doc_language

    def _load_dependencies(self):
        with open(os.path.join(os.path.dirname(__file__), 'dependencies.yaml'), 'r') as file:
            return yaml.safe_load(file)
        return {}

    def build_requirements(self):
        deps = self._load_dependencies()

        if self.options.with_docs:
            self.build_requires(f"doxygen/{deps['doxygen']['version']}")
            self.build_requires(f"protomatlabdoxygen/{deps['protomatlabdoxygen']['20200618_2073e51']}")

        self.build_requires(f"protobuf/{deps['protobuf']['version']}")  # Need protoc on build platform
        if self.options.with_grpc:
            self.build_requires(f"grpc/{deps['grpc']['version']}")  # Need grpc_cpp_plugon on build platform

    def requirements(self):
        # The output library strips all shared library symbols for items not in our public
        # interface, thus all these dependencies are considered private implementation details.
        # It leaves it open for package consumers to define their own versions without symbol
        # conflicts while importing this package.

        deps = self._load_dependencies()

        if self.options.with_tests:
            self.requires(f"gtest/{deps['gtest']['version']}")
            self.requires(f"gflags/{deps['gflags']['version']}")

        if self.options.with_dfxcli:
            self.requires(f"cli11/{deps['cli11']['version']}")
            self.requires(f"naturalsort/{deps['naturalsort']['version']}")

        # Protobuf is a hard requirements to generate sources, not just an override of gRPC
        self.requires(f"protobuf/{deps['protobuf']['version']}")

        # openssl is used by cmake, libwebsockets, gRPC, etc. they tend to drift and we want the latest
        # so it needs to be explicitly set here as the latest.
        if self.options.with_grpc or self.options.with_websocket_json or \
                self.options.with_websocket_protobuf or self.options.with_curl:
            self.requires(f"openssl/{deps['openssl']['version']}", override=True)      # Override grpc dependent on 1.1.1h

        if self.options.with_grpc:
            self.requires(f"abseil/{deps['abseil']['version']}", override=True)
            self.requires(f"re2/{deps['re2']['version']}", override=True)
            self.requires(f"meson/{deps['meson']['version']}", override=True)
            self.requires(f"c-ares/{deps['c-ares']['version']}", override=True)
            self.requires(f"grpc/{deps['grpc']['version']}")

        if self.options.with_curl:
            self.requires(f"libcurl/{deps['libcurl']['version']}")            # Fetch rootca from standalone

        if self.options.with_websocket_json or self.options.with_websocket_protobuf:
            if self.settings.os != "Emscripten":
                self.requires(f"libwebsockets/{deps['libwebsockets']['version']}")
            self.requires(f"base64/{deps['base64']['version']}")

        if self.options.with_yaml:
            self.requires(f"yaml-cpp/{deps['yaml-cpp']['version']}")          # Configuration file handling

        self.requires(f"nlohmann_json/{deps['nlohmann_json']['version']}")
        self.requires(f"fmt/{deps['fmt']['version']}")

        # Nuralogix Conan repository provided
        self.requires(f"dfxprotobufspublic/{deps['dfxprotobufspublic']['version']}")  # gRPC
        self.requires(f"dfxprotosweb/{deps['dfxprotosweb']['version']}") # REST/WebSocket
        self.requires(f"googleapis/{deps['googleapis']['version']}")          # For annotations.proto, http.proto

    def imports(self):
        self.copy(pattern="license*", src="licenses", dst="licenses", folder=True, ignore_case=True)

    def layout(self):
        cmake_layout(self)

    def generate(self):
        toolchain = CMakeToolchain(self)
        toolchain.variables["BUILD_SHARED_LIBS"] = "True" if self.options.shared else "False"
        toolchain.variables["MEASUREMENT_ONLY"] = "ON" if self.options.measurement_only else "OFF"
        toolchain.variables["WITH_CURL"] = "ON" if self.options.with_curl else "OFF"
        toolchain.variables["WITH_REST"] = "ON" if self.options.with_rest else "OFF"
        toolchain.variables["WITH_WEBSOCKET_JSON"] = "ON" if self.options.with_websocket_json else "OFF"
        toolchain.variables["WITH_WEBSOCKET_PROTOBUF"] = "ON" if self.options.with_websocket_protobuf else "OFF"
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
        self.cpp_info.libs = collect_libs(self)

        # Generate DFXCloud-config.cmake
        self.cpp_info.set_property("cmake_file_name", "DFXCloud")

        # DFXCloud:: namespace for the targets
        self.cpp_info.set_property("cmake_target_name", "DFXCloud")

        # Generate both DFXCloud-config.cmake and FindDFXCloud.cmake
        self.cpp_info.set_property("cmake_find_mode", "both")

