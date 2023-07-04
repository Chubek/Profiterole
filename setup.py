from setuptools import Extension, setup
from os import getenv

setup(
    name="pyprofiterole",
    version="1.0",
    author="Chubak Bidpaa",
    author_email="chubakbidpaa@gmail.com",
    url="https://github.com/Chubek/Profiterole",
    description="An overhead-free process profiler with functional interface for Python",
    keywords=[
        "profiler", 
        "unix", 
        "posix", 
        "extension", 
        "c", 
        "python c extension"
        ],
    license="MIT",
    scripts=["scripts/profiterole_binparse.py"],
    
    py_modules=["pyprofiterole.WrapProfiterole"],
    ext_modules=[
        Extension(
            name="profiterole.ExtProfiterole",
            sources=["src/profiler.c", "src/pyprofiterole.c"],
            include_dirs=["include"],
            libraries=["rt"]
        ),
    ]
)