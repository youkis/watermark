/*
The MIT License (MIT)

Copyright (c) 2017 Youki Sada.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include <Python.h>
#include <strings.h>
#include <stdlib.h>
#include "ccc.h"
#include "mls.h"
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

#define MLS__DOC__ ""\
"mls(N,create='one'):\n" \
"N:      demension of monic irreducible polynomial\n" \
"create: generate 'one' or 'full' MLS\n" \
"        or 'preferd' to generate preferd M-sequences\n" \
"Returns one or full Maximum Length Sequences \n"
PyObject *py_mls(PyObject *self, PyObject *args, PyObject* keywds) {
	unsigned n;
  char* create="one";
	static char* kwlist[] = {"N","create",NULL};
	unsigned mls_size=0;
	int **m;
	unsigned N;
	PyObject *mseqs;

	if(!PyArg_ParseTupleAndKeywords(args,keywds,"i|s",kwlist,&n,&create)) return NULL;
	if(strcmp(create,"one")!=0 && strcmp(create,"full")!=0 && strcmp(create,"preferd")!=0){
		fprintf(stderr,"second arg has to be 'one' or 'full' or 'preferd'\n");
		return NULL;
	}

	if(create[0]=='o')      m=mls(n,&mls_size,0);
	else if(create[0]=='f') m=mls(n,&mls_size,1);
	else m=preferd(n,&mls_size);

	N=1<<n;
	if(mls_size==1 && create[0]=='o'){
		npy_intp dims[]={N-1};
		mseqs=PyArray_SimpleNewFromData(1, dims, NPY_INT, *m);
	}else{
		npy_intp dims[]={mls_size,N-1};
		mseqs=PyArray_SimpleNewFromData(2, dims, NPY_INT, *m);
	}
	free(m);
	return mseqs;
}

#define CCC__DOC__ ""\
"ccc(N,seed=123456):\n" \
"N:      CCC(N,N,N**2)\n" \
"seed:   random seed number\n" \
"Returns CCC, which is three dimensional list \n"
PyObject *py_ccc(PyObject *self, PyObject *args, PyObject* keywds) {
	int seed=1234,N;
	static char* kwlist[] = {"N","seed",NULL};
	if(!PyArg_ParseTupleAndKeywords(args,keywds,"i|i",kwlist,&N,&seed)) return NULL;
	int ***CCC=generateCCC(seed,N);
	npy_intp dims[]= {N,N,N*N};
	PyObject *ccc=PyArray_SimpleNewFromData(3, dims, NPY_INT, **CCC);
	free(*CCC);
	free(CCC);
	return ccc;
}

#define PERM__DOC__ ""\
"permutation(size,seed=123):\n" \
"size:   array size\n" \
"seed:   random seed number\n" \
"forword: returns permutation array or its inverse\n" \
"Returns random permutation array \n"
PyObject *py_permutation(PyObject *self, PyObject *args, PyObject* keywds) {
	unsigned size;
  unsigned seed=123;
	static char* kwlist[] = {"size","forward","seed",NULL};
	int forward=1;

	if(!PyArg_ParseTupleAndKeywords(args,keywds,"i|ii",kwlist,&size,&forward,&seed)) return NULL;

	npy_intp dims[]={size};
	if(forward) return PyArray_SimpleNewFromData(1, dims, NPY_UINT, permutation(size,seed));
	else        return PyArray_SimpleNewFromData(1, dims, NPY_UINT, invpermutation(size,seed));
}

static PyMethodDef methods[]={
	{"mls", (PyCFunction)py_mls, METH_VARARGS|METH_KEYWORDS , MLS__DOC__},
	{"ccc", (PyCFunction)py_ccc, METH_VARARGS|METH_KEYWORDS , CCC__DOC__},
	{"permutation", (PyCFunction)py_permutation, METH_VARARGS|METH_KEYWORDS , PERM__DOC__},
	{NULL, NULL} /* sentinel */
};
#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit_sequence(void){
	import_array();
	static struct PyModuleDef cmodule={
		PyModuleDef_HEAD_INIT, "sequence","my functions for steganography\n", -1, methods
	};
	return PyModule_Create(&cmodule);
}
#else
void initsequence(void){
	import_array();
	Py_InitModule3("sequence", methods, "my functions for steganography\n");
}
#endif

