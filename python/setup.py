import os
import sys
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
    extra_compile_args = ["-Wextra"]
    if "--debug" in sys.argv:
        extra_compile_args.extend(["-g", "-UNDEBUG", "-O0"])


# Was hoping pa.get_includes() would help
# but doesn't appear to be the case. For now
# point elsewhere on the system to a pre-built arrow package
# much of this mirrors the build instructions here
# https://arrow.apache.org/docs/developers/python.html#environment-setup-and-build

# pyarrow might support this with the right installation. See
# https://arrow.apache.org/docs/python/extending.html?highlight=import_pyarrow
arrow_include_dir = "../../arrow/python/pyarrow/include"
tableau_include_dir = "../../tableauhyperapi/include"

hyperarrow_module = Extension(
    "libhyperarrow",
    include_dirs=[arrow_include_dir, tableau_include_dir] + ["../src"],
    # TODO: need to figure out a better way to distribute hyperarrow
    # include files as well as libraries; for now hard-coded to
    # expected build folder location
    libraries=pa.get_libraries() + ["hyperarrow_writer"],
    library_dirs=pa.get_library_dirs() + ["../build/src"],
    sources=list(glob("../src/hyperarrow/*.cc")),
    depends=list(glob("../src/hyperarrow/*.h")),
    extra_compile_args=extra_compile_args,
    language="c++",
)

setup(
    name="hyperarrow",
    version="0.0.1-dev",
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
    packages=find_packages(),
    package_data={"": ["*.h"]},
    data_files=[("", ["LICENSE.txt", "README.md"])],
    python_requires=">=3.8",
    install_requires=["pyarrow", "tableauhyperapi"],
    extras_require={"dev": ["pytest"]},
    ext_modules=[hyperarrow_module],
)
