from conan import ConanFile
from conan.tools.files import copy

import os

# just build
#
# to update the contents of this folder with the latest protos

class dfxprotobufspublic(ConanFile):
    name = "dfxprotobufspublic"
    description = "DeepAffex public gRPC protobufs"
    license = "NuraLogix License"

    def export_sources(self):
        copy(self, "*", src=os.path.join(self.recipe_folder, "protos"), dst=os.path.join(self.export_sources_folder, "protos"), keep_path=True)

    def package(self):
        copy(self, "*", src=os.path.join(self.source_folder, "protos"), dst=os.path.join(self.package_folder, "protos"), keep_path=True)

    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []

    def package_id(self):
        self.info.clear()
