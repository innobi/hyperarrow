#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <arrow/python/pyarrow.h>
#include <writer.h>

PyObject *write_to_hyper(PyObject *Py_UNUSED(dummy), PyObject *args) {
  int ok;
  PyObject *arrowTable;

  ok = PyArg_ParseTuple(args, "O", &arrowTable);
  if (!ok)
    return NULL;
  
  if (!writerToHyper(arrowTable)) {
    PyErr_SetString(PyExc_RuntimeError, "Failed to write to hyper file!");
    return NULL;
  }

  Py_RETURN_NONE;
};

static PyMethodDef methods[] = {
  {"write_to_hyper", write_to_hyper, METH_VARARGS,
   "Writes an arrow table to a hyper file"},
  {NULL, NULL, 0, NULL}};


static struct PyModuleDef hyperarrowmodule = {
  .m_base = PyModuleDef_HEAD_INIT,
  .m_name = "libhyperarrow",
  .m_methods = methods};

PyMODINIT_FUNC PyInit_libhyperarrow(void) {
  // import_pyarrow returns 0 on success. otherwise sets PyException
  if (!import_pyarrow()) {
    return PyModule_Create(&hyperarrowmodule);
  }
