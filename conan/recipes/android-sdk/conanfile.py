# -*- coding: utf-8 -*-

from conan import ConanFile, conan_version
from conan.errors import ConanException, ConanInvalidConfiguration
from conan.tools.env import Environment
from conan.tools.files import copy, get
from conan.tools.layout import basic_layout
from conan.tools.scm import Version
from shutil import copytree
import os

# Attempting to run with zulu-openjdk/11.0.15 fails with 'java.lang.NoClassDefFoundError: javax/xml/bind/annotation/XmlSchema'
# when launching the sdkmanager. 
# "Android SDK supports Java 1.7, 1.8, and 1.9"
class AndroidSDKConan(ConanFile):
    min_api_level = 7
    max_api_level = 32
    name = "android-sdk"
    version = "latest"
    description = "Android commandline tools to obtain the Android SDK. It includes the complete set of development and debugging tools for Android"
    url = "https://github.com/Tereius/conan-android-sdk"
    homepage = "https://developer.android.com/studio#cmdline-tools"
    license = "Apache 2.0"
    short_paths = True
    no_copy_source = True
#    build_requires = "zulu-openjdk/8.0.144"
    options = {
        "buildToolsRevision": ["ANY"], 
        "platformVersion": list(range(min_api_level, max_api_level + 1))
    }
    default_options = {
        "buildToolsRevision": "28.0.3", 
        "platformVersion": 24
    }
    settings = "os", "arch"

    def build_requirements(self):
        self.tool_requires("zulu-openjdk/8.70.0.23") #8.0.144")

    @property
    def _arch(self):
        return self.settings.arch

    def __getattr_recursive(self, obj, name, default):
        if obj is None:
            return default
        split_names = name.split('.')
        depth = len(split_names)
        if depth == 1:
            return getattr(obj, name, default)
        return self.__getattr_recursive(getattr(obj, split_names[0], default), ".".join(split_names[1:]), default)

    def get_setting(self, name: str):
        is_build_setting = name.endswith('_build')
        depth = len(name.split('.'))
        settings_target = getattr(self, 'settings_target', None)
        if settings_target is None:
            # It is running in 'host' context
            setting_val = self.__getattr_recursive(self.settings, name, None)
            if setting_val is None:
                raise ConanInvalidConfiguration("Setting in host context with name %s is missing. Make sure to provide it in your conan host profile." % name)
            return setting_val
        else:
            # It is running in 'build' context and it is being used as a build requirement
            setting_name = name.replace('_build', '')
            if is_build_setting:
                setting_val = self.__getattr_recursive(self.settings, setting_name, None)
            else:
                setting_val = self.__getattr_recursive(settings_target, setting_name, None)
            if setting_val is None:
                raise ConanInvalidConfiguration("Setting in build context with name %s is missing. Make sure to provide it in your conan %s profile." % (setting_name, "build" if is_build_setting else "host"))
            return setting_val

    @property
    def sdkmanager_bin(self):
        #return os.path.join(self.source_folder, "cmdline-tools", "bin", "sdkmanager")
        return os.path.join(self.source_folder, "bin", "sdkmanager")

    @staticmethod
    def _chmod_plus_x(filename):
        if os.name == "posix":
            os.chmod(filename, os.stat(filename).st_mode | 0o111)

    def _fix_permissions(self, bin_folder):
        if os.name != "posix":
            return
        for root, _, files in os.walk(bin_folder):
            for filename in files:
                filename = os.path.join(root, filename)
                with open(filename, "rb") as f:
                    sig = f.read(4)
                    if isinstance(sig, str):
                        sig = [ord(s) for s in sig]
                    else:
                        sig = list(sig)
                    if len(sig) > 2 and sig[0] == 0x23 and sig[1] == 0x21:
                        self.output.info(f"chmod on script file: '{filename}'")
                        self._chmod_plus_x(filename)
                    elif sig == [0x7F, 0x45, 0x4C, 0x46]:
                        self.output.info(f"chmod on ELF file: '{filename}'")
                        self._chmod_plus_x(filename)
                    elif sig in (
                        [0xCA, 0xFE, 0xBA, 0xBE],
                        [0xBE, 0xBA, 0xFE, 0xCA],
                        [0xFE, 0xED, 0xFA, 0xCF],
                        [0xCF, 0xFA, 0xED, 0xFE],
                        [0xFE, 0xEF, 0xFA, 0xCE],
                        [0xCE, 0xFA, 0xED, 0xFE]
                    ):
                        self.output.info(f"chmod on Mach-O file: '{filename}'")
                        self._chmod_plus_x(filename)

    def configure(self):
        if int(str(self.options.platformVersion)) < self.min_api_level or int(str(self.options.platformVersion)) > self.max_api_level:
            raise ConanException("Unsupported Android platform version: " + str(self.options.platformVersion) + " (supported [%i ... %i])" % (self.min_api_level, self.max_api_level))
        if self.settings_build.os not in ["Windows", "Macos", "Linux"]:
            raise ConanException("Unsupported build os: %s. Supported are: Windows, Macos, Linux" % self.get_setting("os"))
        if self.settings_build.arch != "x86_64":
            raise ConanException("Unsupported build arch: %s. Supported is: x86_64" % self.get_setting("arch"))

    def layout(self):
        basic_layout(self, src_folder="src")

    def source(self):
        pass

    def generate(self):
        env = Environment()
        env.define_path("ANDROID_USER_HOME", os.path.join(self.build_folder, 'userhome'))
        #env.define_path("SDKMANAGER_OPTS", f"-DANDROID_USER_HOME={os.path.join(self.build_folder, 'userhome')}")
        env.vars(self).save_script("sdk_custom_env")

    def build(self): #self.settings.os in ["Windows",
        get(self, **self.conan_data["sources"][self.version][str(self.settings.os)][str(self._arch)],
            destination=self.source_folder, strip_root=True)
        self._fix_permissions(os.path.join(self.source_folder, "bin"))

        #self.run("echo 'running with conanbuild' && /usr/bin/env", env="conanbuild")

        # sdkmanager does not fully respect ANDROID_USER_HOME? Even when set I get "WARNING /home/USER/.android/repositories.cfg could not be loaded"
        # All you really need is an empty file there, ie. mkdir -p ~/.android && touch ~/.android/repositories.cfg


        self.run(f"yes 2>&1 | {self.sdkmanager_bin} --sdk_root={self.build_folder} --licenses", env="conanbuild")
        self.run(f"{self.sdkmanager_bin} --sdk_root={self.build_folder} --install platforms\\;android-{str(self.options.platformVersion)}", env="conanbuild")
        self.run(f"{self.sdkmanager_bin} --sdk_root={self.build_folder} --install build-tools\\;{str(self.options.buildToolsRevision)}", env="conanbuild")
        self.run(f"{self.sdkmanager_bin} --sdk_root={self.build_folder} --install platform-tools", env="conanbuild")

    sdk_copied = False

    def package(self):
        # Called twice because of 'no_copy_source'. First from source-, then from build-dir
        if not self.sdk_copied:
            copytree(os.path.join(self.build_folder, "build-tools"), os.path.join(self.package_folder, "build-tools"))
            copytree(os.path.join(self.build_folder, "licenses"), os.path.join(self.package_folder, "licenses"))
            copytree(os.path.join(self.build_folder, "platforms"), os.path.join(self.package_folder, "platforms"))
            copytree(os.path.join(self.build_folder, "tools"), os.path.join(self.package_folder, "tools"))
            #copytree(os.path.join(self.build_folder, "userhome"), os.path.join(self.package_folder, "userhome"))
            self.sdk_copied = True

    def package_info(self):
        sdk_root = self.package_folder

        # Environment Variable defintitions: https://stackoverflow.com/a/46976475

        # ANDROID_HOME: Installation directory of SDK (ie. /usr/local/android-sdk) 
        # ANDROID_USER_HOME: Location of SDK related data/user files (ie. ~/.android)

        # ANDROID_NDK_ROOT: Installation directory of NDK (ie. /usr/local/android-ndk)

        # JDK_HOME: Installation location of JDK
        # JAVA_HOME: Installation lcoation of JDK

        # ANDROID_SDK_ROOT: deprecated use ANDROID_HOME
        # ANDROID_SDK_HOME: deprecated use ANDROID_PREFS_ROOT
        # ANDROID_PREFS_ROOT: deprecated use ANDROID_USER_HOME
        # ANDROID_EMULATOR_HOME: Location of emulator specific data (ie. ~/.android)
        # ANDROID_AVD_HOME: Location of AVD-specific data files (ie. ~/.android/avd)

        self.cpp_info.set_property("ANDROID_SDK", sdk_root)  # This is how dfx-embedded-opencv initializes

        self.buildenv_info.define("ANDROID_HOME", sdk_root)
        self.buildenv_info.define("ANDROID_SDK", sdk_root)
        self.buildenv_info.define("ANDROID_USER_HOME", os.path.join(self.package_folder, "userhome"))

        self.buildenv_info.define("ANDROID_SDK_ROOT", sdk_root)     # deprecated
        self.buildenv_info.define("ANDROID_SDK_BUILD_TOOLS_REVISION", str(self.options.buildToolsRevision))
        self.buildenv_info.define("ANDROID_SDK_PLATFORM_VERSION", str(self.options.platformVersion))


        # TODO: conan v1 stuff to remove later
        if Version(conan_version).major < 2:
            self.output.info('Creating SDK_ROOT, ANDROID_SDK_ROOT environment variable: %s' % sdk_root)
            self.env_info.ANDROID_SDK = sdk_root
            self.env_info.SDK_ROOT = sdk_root
            self.env_info.ANDROID_SDK_ROOT = sdk_root
            
            self.env_info.ANDROID_HOME = sdk_root

            self.output.info('Creating ANDROID_BUILD_TOOLS_REVISION environment variable: %s' % str(self.options.buildToolsRevision))
            self.env_info.ANDROID_BUILD_TOOLS_REVISION = str(self.options.buildToolsRevision)
