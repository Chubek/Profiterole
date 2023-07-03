from setuptools import Extension, setup

setup(
    ext_modules=[
        Extension(
            name="pyprofiterole",
            sources=["src/profiler.c", "src/pyprofiterole.c"],
            include_dirs=["include"]
        ),
    ]
)