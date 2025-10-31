from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import os
import subprocess


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the extension")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        
        # Create build directory
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        cmake_args = [
            f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}',
            f'-DPYTHON_EXECUTABLE={sys.executable}',
            '-DBUILD_TESTS=OFF',
        ]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        cmake_args += [f'-DCMAKE_BUILD_TYPE={cfg}']
        build_args += ['--', '-j4']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version()
        )

        subprocess.check_call(
            ['cmake', ext.sourcedir] + cmake_args, 
            cwd=self.build_temp, 
            env=env
        )
        subprocess.check_call(
            ['cmake', '--build', '.'] + build_args, 
            cwd=self.build_temp
        )


setup(
    ext_modules=[CMakeExtension('_fastvoronoi')],
    cmdclass=dict(build_ext=CMakeBuild),
)
