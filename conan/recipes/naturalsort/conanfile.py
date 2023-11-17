from conan import ConanFile
from conan.tools.files import copy

import os

# just build

class NaturalSortConan(ConanFile):
    name = "naturalsort"
    homepage = "https://github.com/scopeInfinity/NaturalSort"
    description = "C++ Header File for Natural Comparison and Natural Sort."
    topics = ("conan", "natural sort", "header-only")
    url = "https://github.com/scopeInfinity/NaturalSort"
    license = "MIT"

    def export_sources(self):
        copy(self, "naturalsort-license.txt", src=os.path.join(self.recipe_folder, "naturalsort"), dst=os.path.join(self.export_sources_folder, "licenses"))
        copy(self, "natural_sort.hpp", src=os.path.join(self.recipe_folder, "naturalsort"), dst=os.path.join(self.export_sources_folder, "include", "natural_sort"));

    def package(self):
        copy(self, "*", src=os.path.join(self.source_folder, "include"), dst=os.path.join(self.package_folder, "include"), keep_path=True)
        copy(self, "*", src=os.path.join(self.source_folder, "licenses"), dst=os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []

    def package_id(self):
        self.info.clear()
