import platform
import os
import pkg_resources
import sys
import sysconfig
from glob import glob

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

# TODO: on Linux we need to set LD_LIBRARY_PATH for auditwheel to copy
# this in, so maybe can leverage that instead of repeating here
# For cross platform, maybe we figure out how to install the tableauhyper lib
tableau_dir = os.environ["HYPER_PATH"]
if not tableau_dir:
    raise ValueError("Must set HYPER_PATH environment variable for python build")

extra_link_args = []
if sys.platform == "darwin":
    lib_location = os.environ["LIB_LOCATION"]
    if not lib_location:
        raise ValueError("must set LIB_LOCATION on macos")
    extra_link_args = ["-rpath", lib_location]

hyperarrow_module = Extension(
    "hyperarrow.libhyperarrow",
    include_dirs=["../include"],
    # TODO: need to figure out a better way to distribute hyperarrow
    # include files as well as libraries; for now hard-coded to
    # expected build folder location
    libraries=["arrow", "arrow_python", "hyperarrow_writer", "hyperarrow_reader", "tableauhyperapi"],
    library_dirs=[tableau_dir + "/lib"],
    extra_link_args=extra_link_args,
    sources=list(glob("src/hyperarrow/hyperarrow.cpp")),
    extra_compile_args=extra_compile_args,
    language="c++",
    py_limited_api=True,
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
    license_files=("LICENSE", "TABLEAU_LICENSE"),
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
    data_files=[("", ["README.md"])],
    python_requires=">=3.8",
    install_requires=[],
    extras_require={"dev": ["pytest"]},
    ext_modules=[hyperarrow_module],
)
