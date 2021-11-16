#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <arrow/python/pyarrow.h>
#include <hyperarrow/writer.h>

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
  hyperarrow::arrowTableToHyper(table, "example.hyper");

  Py_RETURN_NONE;
};

static PyMethodDef methods[] = {
  {"write_to_hyper", write_to_hyper, METH_VARARGS,
   "Writes an arrow table to a hyper file"},
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
  PyDateTime_IMPORT;
  PyObject *module = NULL;
  if (!arrow::py::import_pyarrow()) {
    module = PyModule_Create(&hyperarrowmodule);
  }

  return module;
}
