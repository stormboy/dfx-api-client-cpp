from conan import ConanFile
from conan.tools.files import copy

import os

# just build

class GoogleApis(ConanFile):
    name = "googleapis"
    description = "Public interface definitions of Google APIs. "
    license = "Apache License 2.0"
    url = "https://github.com/googleapis/googleapis"

    def export_sources(self):
        copy(self, "*.proto", os.path.join(self.recipe_folder, "googleapis", "google"), os.path.join(self.export_sources_folder, "googleapis", "google"), keep_path=True)
        copy(self, "LICENSE", os.path.join(self.recipe_folder, "googleapis"), os.path.join(self.export_sources_folder, "googleapis"), keep_path=True)

    def package(self):
        copy(self, "*.proto", os.path.join(self.source_folder, "googleapis", "google"), os.path.join(self.package_folder, "protos", "google"), keep_path=True)
        copy(self, "LICENSE", os.path.join(self.source_folder, "googleapis"), os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        self.cpp_info.resdirs = ["googleapis"]
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []

    def package_id(self):
        self.info.clear()
