from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import subprocess
import os
import sys

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        super().__init__(name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)

    def build_cmake(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cfg = 'Debug' if self.debug else 'Release'
        cmake_args = [
            '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
            '-DPYTHON_EXECUTABLE=' + sys.executable,
            '-DCMAKE_BUILD_TYPE=' + cfg,
            '-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON'
        ]
        build_args = ['--config', cfg]
        
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        
        try:
            subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp)
            subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)
        except subprocess.CalledProcessError as e:
            print(f"Error during CMake execution: {e}")
            sys.exit(1)

setup(
    name='ptar',
    version='0.1',
    author='Simo Ryu',
    author_email='cloneofsimo@gmail.com',
    description='A parallel tar reader using C++ and pybind11',
    long_description='',
    packages=['ptar'],
    ext_modules=[CMakeExtension('ptar', sourcedir='ptar')],
    cmdclass={'build_ext': CMakeBuild},
    zip_safe=False,
)
