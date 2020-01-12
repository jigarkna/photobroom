from conans import ConanFile, CMake

class PhotoBroomConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "Exiv2/0.27@piponazo/stable", "OpenLibrary/2.3@kicer/stable"
    generators = "cmake"
    default_options = {"Exiv2:xmp": False}
