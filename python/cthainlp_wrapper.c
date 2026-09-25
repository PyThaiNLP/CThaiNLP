/**
 * @file cthainlp_wrapper.c
 * @brief Python C extension wrapper for CThaiNLP
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>
#include <stdlib.h>
#include "newmm.h"
#include "tcc.h"
#include "util.h"
#include "soundex.h"

/* Module-level dictionary cache */
static struct {
    newmm_dict_t dict;
    char* dict_path;
} dict_cache = {NULL, NULL};

/**
 * Load or retrieve cached dictionary
 */
static newmm_dict_t get_or_load_dict(const char* dict_path) {
    /* Check if we need to reload the dictionary */
    int need_reload = 0;
    
    if (dict_cache.dict == NULL) {
        /* No cached dict */
        need_reload = 1;
    } else if (dict_path == NULL && dict_cache.dict_path != NULL) {
        /* Switching from custom to default */
        need_reload = 1;
    } else if (dict_path != NULL && dict_cache.dict_path == NULL) {
        /* Switching from default to custom */
        need_reload = 1;
    } else if (dict_path != NULL && dict_cache.dict_path != NULL) {
        /* Both custom, check if path changed */
        if (strcmp(dict_path, dict_cache.dict_path) != 0) {
            need_reload = 1;
        }
    }
    
    if (need_reload) {
        /* Free old dictionary */
        if (dict_cache.dict) {
            newmm_free_dict(dict_cache.dict);
            dict_cache.dict = NULL;
        }
        if (dict_cache.dict_path) {
            free(dict_cache.dict_path);
            dict_cache.dict_path = NULL;
        }
        
        /* Load new dictionary */
        dict_cache.dict = newmm_load_dict(dict_path);
        if (dict_cache.dict && dict_path) {
            dict_cache.dict_path = strdup(dict_path);
            if (!dict_cache.dict_path) {
                /* strdup failed, clean up and return NULL */
                newmm_free_dict(dict_cache.dict);
                dict_cache.dict = NULL;
                return NULL;
            }
        }
    }
    
    return dict_cache.dict;
}

/**
 * Python wrapper for newmm_segment function
 */
static PyObject* py_newmm_segment(PyObject* Py_UNUSED(self), PyObject* args, PyObject* kwargs) {
    const char* text;
    const char* dict_path = NULL;
    int token_count;
    
    /* Parse arguments */
    static char* kwlist[] = {"text", "dict_path", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|z", kwlist, &text, &dict_path)) {
        return NULL;
    }
    
    /* Get or load dictionary */
    newmm_dict_t dict = get_or_load_dict(dict_path);
    if (!dict) {
        PyErr_SetString(PyExc_MemoryError, "Failed to load dictionary (out of memory)");
        return NULL;
    }
    
    /* Call C function with cached dictionary */
    char** tokens = newmm_segment_with_dict(text, dict, &token_count);
    
    if (!tokens) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to segment text");
        return NULL;
    }
    
    /* Convert result to Python list */
    PyObject* result = PyList_New(token_count);
    if (!result) {
        newmm_free_result(tokens, token_count);
        return NULL;
    }
    
    for (int i = 0; i < token_count; i++) {
        PyObject* token_str = PyUnicode_FromString(tokens[i]);
        if (!token_str) {
            Py_DECREF(result);
            newmm_free_result(tokens, token_count);
            return NULL;
        }
        PyList_SET_ITEM(result, i, token_str);
    }
    
    /* Free C memory */
    newmm_free_result(tokens, token_count);
    
    return result;
}

/**
 * Clear cached dictionary
 */
static PyObject* py_clear_cache(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(args)) {
    if (dict_cache.dict) {
        newmm_free_dict(dict_cache.dict);
        dict_cache.dict = NULL;
    }
    if (dict_cache.dict_path) {
        free(dict_cache.dict_path);
        dict_cache.dict_path = NULL;
    }
    Py_RETURN_NONE;
}

/**
 * Python wrapper for tcc_segment
 */
static PyObject* py_tcc_segment(PyObject* Py_UNUSED(self), PyObject* args) {
    const char* text;
    if (!PyArg_ParseTuple(args, "s", &text)) {
        return NULL;
    }
    
    if (!text || text[0] == '\0') {
        return PyList_New(0);
    }
    
    int token_count = 0;
    char** tokens = tcc_segment(text, &token_count);
    if (!tokens) {
        return PyList_New(0);
    }
    
    PyObject* result = PyList_New(token_count);
    if (!result) {
        tcc_free_result(tokens, token_count);
        return NULL;
    }
    
    for (int i = 0; i < token_count; i++) {
        PyObject* token_str = PyUnicode_FromString(tokens[i]);
        if (!token_str) {
            Py_DECREF(result);
            tcc_free_result(tokens, token_count);
            return NULL;
        }
        PyList_SET_ITEM(result, i, token_str);
    }
    
    tcc_free_result(tokens, token_count);
    return result;
}

/**
 * Python wrapper for tcc_pos
 */
static PyObject* py_tcc_pos(PyObject* Py_UNUSED(self), PyObject* args) {
    const char* text;
    if (!PyArg_ParseTuple(args, "s", &text)) {
        return NULL;
    }
    
    if (!text || text[0] == '\0') {
        return PyList_New(0);
    }
    
    int* positions = NULL;
    int count = tcc_pos(text, &positions);
    if (count <= 0 || !positions) {
        return PyList_New(0);
    }
    
    PyObject* result = PyList_New(count);
    if (!result) {
        free(positions);
        return NULL;
    }
    
    for (int i = 0; i < count; i++) {
        PyObject* pos_val = PyLong_FromLong(positions[i]);
        if (!pos_val) {
            Py_DECREF(result);
            free(positions);
            return NULL;
        }
        PyList_SET_ITEM(result, i, pos_val);
    }
    
    free(positions);
    return result;
}

/**
 * Python wrapper for is_thai_char
 */
static PyObject* py_is_thai_char(PyObject* Py_UNUSED(self), PyObject* args) {
    const char* ch;
    if (!PyArg_ParseTuple(args, "s", &ch)) {
        return NULL;
    }
    return PyBool_FromLong(is_thai_char(ch));
}

/**
 * Python wrapper for is_thai
 */
static PyObject* py_is_thai(PyObject* Py_UNUSED(self), PyObject* args, PyObject* kwargs) {
    const char* text;
    const char* ignore_chars = ".";
    
    static char* kwlist[] = {"text", "ignore_chars", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|z", kwlist, &text, &ignore_chars)) {
        return NULL;
    }
    
    return PyBool_FromLong(is_thai(text, ignore_chars));
}

/**
 * Python wrapper for count_thai
 */
static PyObject* py_count_thai(PyObject* Py_UNUSED(self), PyObject* args, PyObject* kwargs) {
    const char* text;
    const char* ignore_chars = NULL;
    
    static char* kwlist[] = {"text", "ignore_chars", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|z", kwlist, &text, &ignore_chars)) {
        return NULL;
    }
    
    double result = count_thai(text, ignore_chars);
    return PyFloat_FromDouble(result);
}

/**
 * Python wrapper for arabic_digit_to_thai_digit
 */
static PyObject* py_arabic_digit_to_thai_digit(PyObject* Py_UNUSED(self), PyObject* args) {
    const char* text;
    if (!PyArg_ParseTuple(args, "s", &text)) {
        return NULL;
    }
    char* result = arabic_digit_to_thai_digit(text);
    if (!result) {
        Py_RETURN_NONE;
    }
    PyObject* py_res = PyUnicode_FromString(result);
    cthainlp_free_string(result);
    return py_res;
}

/**
 * Python wrapper for thai_digit_to_arabic_digit
 */
static PyObject* py_thai_digit_to_arabic_digit(PyObject* Py_UNUSED(self), PyObject* args) {
    const char* text;
    if (!PyArg_ParseTuple(args, "s", &text)) {
        return NULL;
    }
    char* result = thai_digit_to_arabic_digit(text);
    if (!result) {
        Py_RETURN_NONE;
    }
    PyObject* py_res = PyUnicode_FromString(result);
    cthainlp_free_string(result);
    return py_res;
}

/**
 * Python wrapper for remove_tonemark
 */
static PyObject* py_remove_tonemark(PyObject* Py_UNUSED(self), PyObject* args) {
    const char* text;
    if (!PyArg_ParseTuple(args, "s", &text)) {
        return NULL;
    }
    char* result = remove_tonemark(text);
    if (!result) {
        Py_RETURN_NONE;
    }
    PyObject* py_res = PyUnicode_FromString(result);
    cthainlp_free_string(result);
    return py_res;
}

/**
 * Python wrapper for soundex_lk82
 */
static PyObject* py_soundex_lk82(PyObject* Py_UNUSED(self), PyObject* args) {
    const char* text;
    if (!PyArg_ParseTuple(args, "s", &text)) {
        return NULL;
    }
    char* result = soundex_lk82(text);
    if (!result) {
        return PyUnicode_FromString("");
    }
    PyObject* py_res = PyUnicode_FromString(result);
    soundex_free(result);
    return py_res;
}

/**
 * Python wrapper for soundex_udom83
 */
static PyObject* py_soundex_udom83(PyObject* Py_UNUSED(self), PyObject* args) {
    const char* text;
    if (!PyArg_ParseTuple(args, "s", &text)) {
        return NULL;
    }
    char* result = soundex_udom83(text);
    if (!result) {
        return PyUnicode_FromString("");
    }
    PyObject* py_res = PyUnicode_FromString(result);
    soundex_free(result);
    return py_res;
}

/**
 * Module method definitions
 */
static PyMethodDef CThaiNLPMethods[] = {
    {
        "segment",
        (PyCFunction)(void(*)(void))py_newmm_segment,
        METH_VARARGS | METH_KEYWORDS,
        "Segment Thai text into words using newmm algorithm.\n"
    },
    {
        "clear_cache",
        py_clear_cache,
        METH_NOARGS,
        "Clear the cached dictionary.\n"
    },
    {
        "tcc_segment",
        py_tcc_segment,
        METH_VARARGS,
        "Segment Thai text into Thai Character Clusters (TCC).\n"
    },
    {
        "tcc_pos",
        py_tcc_pos,
        METH_VARARGS,
        "Get ending byte positions of Thai Character Clusters.\n"
    },
    {
        "is_thai_char",
        py_is_thai_char,
        METH_VARARGS,
        "Check if character is a Thai character.\n"
    },
    {
        "is_thai",
        (PyCFunction)(void(*)(void))py_is_thai,
        METH_VARARGS | METH_KEYWORDS,
        "Check if every character in text is a Thai character.\n"
    },
    {
        "count_thai",
        (PyCFunction)(void(*)(void))py_count_thai,
        METH_VARARGS | METH_KEYWORDS,
        "Calculate percentage proportion of Thai characters.\n"
    },
    {
        "arabic_digit_to_thai_digit",
        py_arabic_digit_to_thai_digit,
        METH_VARARGS,
        "Convert Arabic digits to Thai digits.\n"
    },
    {
        "thai_digit_to_arabic_digit",
        py_thai_digit_to_arabic_digit,
        METH_VARARGS,
        "Convert Thai digits to Arabic digits.\n"
    },
    {
        "remove_tonemark",
        py_remove_tonemark,
        METH_VARARGS,
        "Remove Thai tone marks from text.\n"
    },
    {
        "soundex_lk82",
        py_soundex_lk82,
        METH_VARARGS,
        "Calculate LK82 soundex code for Thai text.\n"
    },
    {
        "soundex_udom83",
        py_soundex_udom83,
        METH_VARARGS,
        "Calculate Udom83 soundex code for Thai text.\n"
    },
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

/**
 * Module definition
 */
static struct PyModuleDef cthainlp_module = {
    PyModuleDef_HEAD_INIT,
    "_cthainlp",
    "CThaiNLP - Thai Natural Language Processing C extension module",
    -1,
    CThaiNLPMethods,
    NULL,  /* m_slots */
    NULL,  /* m_traverse */
    NULL,  /* m_clear */
    NULL   /* m_free */
};

/**
 * Module cleanup function
 */
static void module_free(void* Py_UNUSED(self)) {
    /* Clean up cached dictionary on module unload */
    if (dict_cache.dict) {
        newmm_free_dict(dict_cache.dict);
        dict_cache.dict = NULL;
    }
    if (dict_cache.dict_path) {
        free(dict_cache.dict_path);
        dict_cache.dict_path = NULL;
    }
}

/**
 * Module initialization function
 */
PyMODINIT_FUNC PyInit__cthainlp(void) {
    /* Update module definition with cleanup function */
    cthainlp_module.m_free = module_free;
    return PyModule_Create(&cthainlp_module);
}
