import platform
import os
import pkg_resources
import sys
import sysconfig
from glob import glob

import pyarrow as pa
from setuptools import Extension, find_packages, setup

here = os.path.abspath(os.path.dirname(__file__))

with open(os.path.join(here, "README.md"), encoding="utf-8") as f:
    long_description = f.read()


# MSVC compiler has different flags; assume that's what we are using on Windows
if os.name == "nt":
    # Enable extra warnings except implicit cast, which throws a few
    # see https://bugzilla.mozilla.org/show_bug.cgi?id=857863 for justification
    extra_compile_args = ["/WX", "/wd4244"]
else:
    # Would love to add -Werror here but looks like the tableauhyperapi
    # ships with a few
    extra_compile_args = ["-Wextra", "-std=c++11"]
    if "--debug" in sys.argv:
        extra_compile_args.extend(["-g", "-UNDEBUG", "-O0"])

# pyarrow might support this with the right installation. See
# https://arrow.apache.org/docs/python/extending.html?highlight=import_pyarrow
tableau_include_dir = "../../tableauhyperapi/include"

extra_link_args = []
package_data = ["lib/*.so", "lib/*.dylib", "lib/*.lib", "./*.dll"]
if sys.platform == "darwin":
    extra_link_args.append("-Wl,-rpath,@loader_path/lib/.")
    # loop required by delocate until we implement pkg-config?
    # https://github.com/matthew-brett/delocate/issues/131
    for libdir in pa.get_library_dirs():
        extra_link_args.append(f"-Wl,{lib_dir}")
elif sys.platform == "linux":
    extra_link_args.append("-Wl,-rpath=$ORIGIN/lib/.")
elif sys.platform == "win32":
    extra_link_args.append("/libpath:lib")
else:
    raise ValueError("Unsupported platform")


# Inspiration for this method taken from:
# https://stackoverflow.com/a/63837811/621736

def path_to_build_folder():
    """Returns the name of a distutils build directory"""
    f = "{dirname}.{platform}-{version[0]}.{version[1]}"
    dir_name = f.format(dirname='lib',
                    platform=sysconfig.get_platform(),
                    version=sys.version_info)
    return os.path.join('build', dir_name, 'hyperarrow')

hyperarrow_module = Extension(
    "hyperarrow.libhyperarrow",
    include_dirs=[pa.get_include(), tableau_include_dir, "../include"],
    # TODO: need to figure out a better way to distribute hyperarrow
    # include files as well as libraries; for now hard-coded to
    # expected build folder location
    libraries=pa.get_libraries() + ["hyperarrow_writer", "hyperarrow_reader"],
    library_dirs=pa.get_library_dirs() + [os.path.join(path_to_build_folder(), "lib")],
    sources=list(glob("src/hyperarrow/hyperarrow.cpp")),
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
    language="c++",
)

setup(
    name="hyperarrow",
    version="0.0.1.dev0",
    description="Python wrapper to hyperarrow",
    long_description=long_description,
    long_description_content_type="text/markdown",    
    url="https://github.com/innobi/hyperarrow",
    author="Will Ayd",
    author_email="will_ayd@innobi.io",
    license="Apache",
    classifiers=[
        "Development Status :: 3 - Alpha",
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: Apache 2.0 License',
        'Programming Language :: Python :: 3'
    ],
    keywords="tableau tableauhyperapi arrow",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    package_data={"hyperarrow": package_data},
    data_files=[("", ["README.md"])],
    python_requires=">=3.8",
    install_requires=["pyarrow", "tableauhyperapi"],
    extras_require={"dev": ["pytest"]},
    ext_modules=[hyperarrow_module],
)
