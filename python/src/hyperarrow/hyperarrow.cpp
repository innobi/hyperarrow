#define Py_LIMITED_API 0x03080000
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <arrow/python/pyarrow.h>
#include <hyperarrow/writer.h>
#include <hyperarrow/reader.h>

static PyObject *write_to_hyper(PyObject *Py_UNUSED(dummy), PyObject *args) {
  int ok;
  PyObject *obj;

  ok = PyArg_ParseTuple(args, "O", &obj);
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
  // TODO: this should probably return some kind of status code
  hyperarrow::arrowTableToHyper(table, "example.hyper", "schema", "table");

  Py_RETURN_NONE;
};

static PyObject *read_from_hyper(PyObject *Py_UNUSED(dummy), PyObject *args) {
  PyObject *obj;

  auto result = hyperarrow::arrowTableFromHyper("example.hyper", "schema", "table");
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
  .m_base = PyModuleDef_HEAD_INIT,
  .m_name = "libhyperarrow",
  .m_doc = PyDoc_STR("Writes pyarrow Table to a hyper file"),
  .m_size = 0,
  .m_methods = methods,
  .m_slots = NULL,
  .m_traverse = NULL,
  .m_clear = NULL,
  .m_free = NULL
};

PyMODINIT_FUNC
PyInit_libhyperarrow(void) {
  PyObject *module = NULL;
  if (!arrow::py::import_pyarrow()) {
    module = PyModule_Create(&hyperarrowmodule);
  }

  return module;
}
