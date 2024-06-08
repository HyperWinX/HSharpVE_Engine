from conan import ConanFile
from conan.tools.cmake import cmake_layout

def match_versions(dep: str) -> str:
    deps = {
        'plog': '1.1.10',
        'gtest': '1.14.0',
        'nlohmann_json': '3.11.3'
    }

    return deps[dep]

class HSharp(ConanFile):
    # profile
    version = '0.0a'
    generators = ['CMakeToolchain', 'CMakeDeps']
    settings = 'os', 'compiler', 'build_type', 'arch'

    def requirements(self):
        for req in [f'{dep}/{match_versions(dep)}' for dep in [
            'plog', 'gtest', 'nlohmann_json'
        ]]:
            self.requires(req)

    def layout(self):
        cmake_layout(self)