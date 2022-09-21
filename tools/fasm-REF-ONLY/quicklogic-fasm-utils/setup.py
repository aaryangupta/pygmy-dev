import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="fasm_utils",
    version="0.0.1",
    packages=setuptools.find_packages(),
)
