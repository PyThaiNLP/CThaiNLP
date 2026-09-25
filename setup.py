"""Setup script for CThaiNLP Python bindings."""

from setuptools import setup, Extension, find_packages
import os
import sys

# Read README for long description
with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

# Platform-specific compiler flags
if sys.platform == "win32":
    # MSVC compiler flags
    extra_compile_args = ["/W3", "/O2"]
else:
    # GCC/Clang compiler flags
    extra_compile_args = ["-Wall", "-Wextra", "-O2"]

# Define the C extension module
cthainlp_extension = Extension(
    name="_cthainlp",
    sources=[
        "src/trie.c",
        "src/tcc.c",
        "src/newmm.c",
        "src/util.c",
        "src/soundex.c",
        "python/cthainlp_wrapper.c",
    ],
    include_dirs=["include"],
    extra_compile_args=extra_compile_args,
)

setup(
    name="cthainlp",
    version="0.1.0",
    author="Wannaphong Phatthiyaphaibun",
    author_email="",
    description="Python bindings for CThaiNLP - Thai Natural Language Processing library in C",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/wannaphong/CThaiNLP",
    packages=find_packages(),
    ext_modules=[cthainlp_extension],
    package_data={
        "cthainlp": ["data/*.txt"],
    },
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Apache Software License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: C",
        "Topic :: Text Processing :: Linguistic",
    ],
    python_requires=">=3.8",
    include_package_data=True,
)
