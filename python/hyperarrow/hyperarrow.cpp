#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <arrow/python/pyarrow.h>
#include <hyperarrow/writer.h>
#include <hyperarrow/reader.h>

static PyObject *write_to_hyper(PyObject *Py_UNUSED(dummy), PyObject *args) {
  int ok;
  PyObject *obj;
  const char *path, *schema, *tableName;

  ok = PyArg_ParseTuple(args, "Osss", &obj, &path, &schema, &tableName);
  if (!ok)
    return NULL;

  if (!arrow::py::is_table(obj)) {
    return NULL;
  }

  auto maybe_table = arrow::py::unwrap_table(obj);
  if (!maybe_table.ok()) {
    return NULL;
  }
  auto table = maybe_table.ValueOrDie();
  try {
    hyperarrow::arrowTableToHyper(table, path, schema, tableName);    
  } catch(const std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return NULL;
  }  

  Py_RETURN_NONE;
};

static PyObject *read_from_hyper(PyObject *Py_UNUSED(dummy), PyObject *args) {
  int ok;
  PyObject *obj;
  const char *path, *schema, *tableName;

  ok = PyArg_ParseTuple(args, "sss", &path, &schema, &tableName);
  if (!ok)
    return NULL;

  arrow::Result<std::shared_ptr<arrow::Table>> result;
  try {
    result = hyperarrow::arrowTableFromHyper(path, schema, tableName);
  } catch(const std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return NULL;
  }
  if (result.ok()) {
    auto table = result.ValueOrDie();
    obj = arrow::py::wrap_table(table);
  } else {
    PyErr_SetString(PyExc_RuntimeError, "Could not read hyper file");
    obj = NULL;
  }

  return obj;
}

static PyMethodDef methods[] = {
  {"write_to_hyper", write_to_hyper, METH_VARARGS,
   "Writes an arrow table to a hyper file"},
  {"read_from_hyper", read_from_hyper, METH_VARARGS,
   "Reads an arrow table from a hyper file"},
  {NULL, NULL, 0, NULL}};


static struct PyModuleDef hyperarrowmodule = {
  PyModuleDef_HEAD_INIT,	// m_base
  "libhyperarrow",		// m_name
  PyDoc_STR("Writes pyarrow Table to a hyper file"),	// m_doc
  0,				// m_size
  methods,			// m_methods
  NULL,				// m_slots
  NULL,				// m_traverse
  NULL,				// m_clear
  NULL				// m_free
};

PyMODINIT_FUNC
PyInit_libhyperarrow(void) {
  PyObject *module = NULL;
  if (!arrow::py::import_pyarrow()) {
    module = PyModule_Create(&hyperarrowmodule);
  }

  return module;
}
