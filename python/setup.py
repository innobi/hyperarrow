import os
import sys
from glob import glob

import numpy as np
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
    extra_compile_args = ["-Wextra", "-Werror"]
    if "--debug" in sys.argv:
        extra_compile_args.extend(["-g", "-UNDEBUG", "-O0"])

hyperarrow_module = Extension(
    "libhyperarrow",
    include_dirs=[pa.get_include()],
    sources=list(glob("hyperarrow/src/*.c")),
    depends=list(glob("hyperarrow/src/*.h")),
    extra_compile_args=extra_compile_args,
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