from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup
import pybind11

# Define the extension module
ext_modules = [
    Pybind11Extension(
        "worldgen_cpp",
        [
            "worldgen_wrapper.cpp",
            "../../engine/src/world/procedural_terrain/noise/multi_scale_noise.cpp",
            "../../engine/src/world/procedural_terrain/noise/fractal_patterns.cpp",
            "../../engine/src/world/procedural_terrain/utils/seed_utils.cpp",
            "../../engine/src/world/procedural_terrain/climate/climate_system.cpp",
        ],
        include_dirs=[
            "../../engine/include",
            pybind11.get_cmake_dir() + "/../../../include",
        ],
        cxx_std=17,
    ),
]

setup(
    name="worldgen_cpp",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.6",
)
