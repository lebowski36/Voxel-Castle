#include <Python.h>
#include <numpy/arrayobject.h>
#include "../../engine/include/world/procedural_terrain/noise/multi_scale_noise.h"
#include "../../engine/include/world/procedural_terrain/utils/seed_utils.h"

using namespace VoxelCastle::World::ProceduralTerrain;

// Python wrapper for MultiScaleNoise::GenerateHeightmapNoise
static PyObject* generate_heightmap_noise(PyObject* self, PyObject* args) {
    float x, z;
    unsigned long long seed;
    
    if (!PyArg_ParseTuple(args, "ffK", &x, &z, &seed)) {
        return NULL;
    }
    
    float result = MultiScaleNoise::GenerateHeightmapNoise(x, z, seed);
    return PyFloat_FromDouble(result);
}

// Python wrapper for MultiScaleNoise::GenerateCombinedNoise
static PyObject* generate_combined_noise(PyObject* self, PyObject* args) {
    float x, z;
    unsigned long long seed;
    float continental_weight = 0.4f;
    float regional_weight = 0.3f;
    float local_weight = 0.2f;
    float micro_weight = 0.1f;
    
    if (!PyArg_ParseTuple(args, "ffK|ffff", &x, &z, &seed, 
                         &continental_weight, &regional_weight, 
                         &local_weight, &micro_weight)) {
        return NULL;
    }
    
    float result = MultiScaleNoise::GenerateCombinedNoise(x, z, seed,
                                                          continental_weight,
                                                          regional_weight,
                                                          local_weight,
                                                          micro_weight);
    return PyFloat_FromDouble(result);
}

// Python wrapper for MultiScaleNoise::GenerateRidgeNoise
static PyObject* generate_ridge_noise(PyObject* self, PyObject* args) {
    float x, z;
    unsigned long long seed;
    
    if (!PyArg_ParseTuple(args, "ffK", &x, &z, &seed)) {
        return NULL;
    }
    
    float result = MultiScaleNoise::GenerateRidgeNoise(x, z, seed);
    return PyFloat_FromDouble(result);
}

// Batch generation for efficiency
static PyObject* generate_heightmap_batch(PyObject* self, PyObject* args) {
    PyObject* x_array_obj;
    PyObject* z_array_obj;
    unsigned long long seed;
    
    if (!PyArg_ParseTuple(args, "OOK", &x_array_obj, &z_array_obj, &seed)) {
        return NULL;
    }
    
    PyArrayObject* x_array = (PyArrayObject*)PyArray_FROM_OTF(x_array_obj, NPY_FLOAT32, NPY_ARRAY_IN_ARRAY);
    PyArrayObject* z_array = (PyArrayObject*)PyArray_FROM_OTF(z_array_obj, NPY_FLOAT32, NPY_ARRAY_IN_ARRAY);
    
    if (x_array == NULL || z_array == NULL) {
        Py_XDECREF(x_array);
        Py_XDECREF(z_array);
        return NULL;
    }
    
    npy_intp size = PyArray_SIZE(x_array);
    if (PyArray_SIZE(z_array) != size) {
        PyErr_SetString(PyExc_ValueError, "x and z arrays must have the same size");
        Py_DECREF(x_array);
        Py_DECREF(z_array);
        return NULL;
    }
    
    // Create output array
    PyArrayObject* result_array = (PyArrayObject*)PyArray_SimpleNew(1, &size, NPY_FLOAT32);
    if (result_array == NULL) {
        Py_DECREF(x_array);
        Py_DECREF(z_array);
        return NULL;
    }
    
    float* x_data = (float*)PyArray_DATA(x_array);
    float* z_data = (float*)PyArray_DATA(z_array);
    float* result_data = (float*)PyArray_DATA(result_array);
    
    // Generate noise for each point
    for (npy_intp i = 0; i < size; i++) {
        result_data[i] = MultiScaleNoise::GenerateHeightmapNoise(x_data[i], z_data[i], seed);
    }
    
    Py_DECREF(x_array);
    Py_DECREF(z_array);
    
    return (PyObject*)result_array;
}

// Method definitions
static PyMethodDef TerrainMethods[] = {
    {"generate_heightmap_noise", generate_heightmap_noise, METH_VARARGS,
     "Generate heightmap noise at a single point"},
    {"generate_combined_noise", generate_combined_noise, METH_VARARGS,
     "Generate combined multi-scale noise"},
    {"generate_ridge_noise", generate_ridge_noise, METH_VARARGS,
     "Generate ridge noise for mountain ranges"},
    {"generate_heightmap_batch", generate_heightmap_batch, METH_VARARGS,
     "Generate heightmap noise for arrays of points (efficient)"},
    {NULL, NULL, 0, NULL}
};

// Module definition
static struct PyModuleDef terrainmodule = {
    PyModuleDef_HEAD_INIT,
    "terrain_cpp",
    "Python wrapper for C++ terrain generation",
    -1,
    TerrainMethods
};

// Module initialization
PyMODINIT_FUNC PyInit_terrain_cpp(void) {
    import_array();  // Initialize NumPy
    return PyModule_Create(&terrainmodule);
}
