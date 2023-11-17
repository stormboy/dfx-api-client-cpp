from conan import ConanFile
from conan.tools.files import copy

import os

# just build
#
# to update the contents of this folder with the latest protos

class protomatlabdoxygen(ConanFile):
    name = "protomatlabdoxygen"
    description = "Doxygen Filters for Proto Matlab"
    license = "LGPL+BSD2"
    url = "https://github.com/aphysci/proto-matlab-doxygen"

    def export_sources(self):
        copy(self, "*", src=os.path.join(self.recipe_folder, "bin"), dst=os.path.join(self.export_sources_folder, "bin"), keep_path=True)
        copy(self, "*", src=os.path.join(self.recipe_folder, "license"), dst=os.path.join(self.export_sources_folder, "license"), keep_path=True)

    def package(self):
        # proto2cpp.py: for protobuf to cpp (requires Python)
        # m2cpp.pl:     for matlab to cpp (requires Perl)
        copy(self, "*", src=os.path.join(self.source_folder, "bin"), dst=os.path.join(self.package_folder, "bin"), keep_path=True)
        copy(self, "*", src=os.path.join(self.source_folder, "license"), dst=os.path.join(self.package_folder, "license"), keep_path=True)

    def package_info(self):
        self.cpp_info.bindirs = ["bin"]
        self.cpp_info.libdirs = []
        self.env_info.path.append(os.path.join(self.package_folder, "bin"))

    def package_id(self):
        self.info.clear()

