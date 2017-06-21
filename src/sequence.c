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
//#include <numpy/arrayobject.h>

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
	char **m;
	unsigned N;
	unsigned i,j;
	PyObject *mseqs;

	if(!PyArg_ParseTupleAndKeywords(args,keywds,"i|s",kwlist,&n,&create)) return NULL;
	if(strcmp(create,"one")!=0 && strcmp(create,"full")!=0 && strcmp(create,"preferd")!=0){
		fprintf(stderr,"second arg has to be 'one' or 'full' or 'preferd'\n");
		return NULL;
	}

	if(create[0]=='o')      m=mls(n,&mls_size,0);
	else if(create[0]=='f') m=mls(n,&mls_size,1);
	else m=preferd(n,&mls_size);

	mseqs=PyList_New(mls_size);
	N=1<<n;

	for(i=0;i<mls_size;i++){
		PyObject *mseq=PyList_New(N-1);
		for(j=0;j<N-1;j++) PyList_SET_ITEM(mseq,j,Py_BuildValue("i",m[i][j]));
		PyList_SET_ITEM(mseqs,i,mseq);
	}

	free(*m);
	free(m);

	if(mls_size==1) return PyList_GET_ITEM(mseqs,0);
	return mseqs;
}

#define CCC__DOC__ ""\
"ccc(N,seed=123456):\n" \
"N:      CCC(N,N,N**2)\n" \
"seed:   random seed number\n" \
"Returns CCC, which is three dimensional list \n"
PyObject *py_ccc(PyObject *self, PyObject *args, PyObject* keywds) {
	int seed=123456,N;
	int i,j,k;
	static char* kwlist[] = {"N","seed",NULL};
	if(!PyArg_ParseTupleAndKeywords(args,keywds,"i|i",kwlist,&N,&seed)) return NULL;
	char ***CCC=generateCCC(seed,N);
	PyObject *ccc=PyList_New(N);
	for(i=0;i<N;i++){
		PyObject *cc=PyList_New(N);
		for(j=0;j<N;j++){
			PyObject *c=PyList_New(N*N);
			for(k=0;k<N*N;k++) PyList_SET_ITEM(c,k,Py_BuildValue("i",CCC[i][j][k]));
			PyList_SET_ITEM(cc,j,c);
		}
		PyList_SET_ITEM(ccc,i,cc);
	}
	free(**CCC);
	free(*CCC);
	free(CCC);
	return ccc;
}

#define GETBASESEQUENCE__DOC__ ""\
"getBaseSequence(CCC,datasize,shift,ch):\n" \
"CCC:      CCC(N,N,N**2)\n" \
"datasize: data size [bit]\n" \
"shift:    shift size when you're gonna make embed sequence [tip]\n" \
"ch:       CCC chanel to use (1-N)\n" \
"Returns base sequence \n"
PyObject *getBaseSequence(PyObject **self, PyObject *args, PyObject* keywds){
	static char* kwlist[] = {"CCC","datasize","shift","ch",NULL};
	PyObject *ccc;
	int ch=1,shift,datasize;
	if(!PyArg_ParseTupleAndKeywords(args,keywds,"Oii|i",kwlist,&ccc,&datasize,&shift,&ch)) return NULL;
	unsigned N=PyList_GET_SIZE(ccc);
	unsigned NN=N*N;
	unsigned zero=shift*(datasize-1);//ゼロ埋めの数
	unsigned i,j;
	PyObject *s=PyList_New((int)(N*N*N+zero*(N-1)));//メモリコンフリクトする
	for(i=0;i<N;i++){
		char ss=(NN+zero)*i;
		PyObject *c=PyList_GET_ITEM(PyList_GET_ITEM(ccc,ch-1),i);
		for(j=0;j<NN;j++) PyList_SET_ITEM(s,ss+j,PyList_GET_ITEM(c,j));
		for(j=NN;j<NN+zero;j++) PyList_SET_ITEM(s,ss+j,Py_BuildValue("i",0));
	}
	return s;
}
#define GETEMBEDSEQUENCE__DOC__ ""\
"getEmbedSequence(base,data,N,shift):\n" \
"base:   base sequence\n" \
"data:   bit list data such as [1,0,0,0,1,1].\n" \
"N:      CCC size.\n" \
"shift:  the shift value when the base sequence is convoluted. [tip]\n" \
"Returns embed sequence.\n"
PyObject *getEmbedSequence(PyObject **self, PyObject *args, PyObject* keywds){
	static char* kwlist[] = {"base","data","N","shift",NULL};
	PyObject *s,*d;
	unsigned shift,N;
	if(!PyArg_ParseTupleAndKeywords(args,keywds,"OOii",kwlist,&s,&d,&N,&shift)) return NULL;
	unsigned datasize=PyList_GET_SIZE(d);
	unsigned zero=shift*(datasize-1);//ゼロ埋めの数
	unsigned n=N*N*N+zero*(N-1); //基礎系列sの長さ 最後のゼロ埋めは不要
	unsigned NN=N*N;
	int *y=(int *)calloc(n+zero,sizeof(int)); //埋め込み系列
	PyObject *yy=PyList_New((int)(n+zero)); //埋め込み系列 pylist
	unsigned i,j,k;
	for(i=0;i<datasize;i++){
		int *yit=y+i*shift;
		int di=PyLong_AsLong(PyList_GET_ITEM(d,i))*2-1;
		for(j=0;j<N;j++){
			int jNz=j*(NN+zero);
			for(k=jNz;k<jNz+NN;k++) yit[k]+=di*PyLong_AsLong(PyList_GET_ITEM(s,k));
		}
	}
	for(i=0;i<n+zero;i++) PyList_SET_ITEM(yy,i,Py_BuildValue("i",y[i]));
	free(y);
	return yy;
	//return Py_BuildValue("i",1);
}

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit_sequence(void){
	static PyMethodDef methods[]={
		{"mls", (PyCFunction)py_mls, METH_VARARGS|METH_KEYWORDS , MLS__DOC__},
		{"ccc", (PyCFunction)py_ccc, METH_VARARGS|METH_KEYWORDS , CCC__DOC__},
		{"getBaseSequence" , (PyCFunction)getBaseSequence , METH_VARARGS|METH_KEYWORDS , GETBASESEQUENCE__DOC__},
		{"getEmbedSequence", (PyCFunction)getEmbedSequence, METH_VARARGS|METH_KEYWORDS , GETEMBEDSEQUENCE__DOC__},
		{NULL, NULL} /* sentinel */
	};
	static struct PyModuleDef cmodule = {
		PyModuleDef_HEAD_INIT,
		"sequence",     /* name of module */
		"my functions for steganography\n", /* module documentation, may be NULL */
		-1,      /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
		methods
	};
	return PyModule_Create(&cmodule);
}
#else
void initsequence(void){
	static PyMethodDef methods[]={
		{"mls", (PyCFunction)py_mls, METH_VARARGS|METH_KEYWORDS , MLS__DOC__},
		{"ccc", (PyCFunction)py_ccc, METH_VARARGS|METH_KEYWORDS , CCC__DOC__},
		{"getBaseSequence" , (PyCFunction)getBaseSequence , METH_VARARGS|METH_KEYWORDS , GETBASESEQUENCE__DOC__},
		{"getEmbedSequence", (PyCFunction)getEmbedSequence, METH_VARARGS|METH_KEYWORDS , GETEMBEDSEQUENCE__DOC__},
		{NULL, NULL} /* sentinel */
	};
	Py_InitModule3("sequence", methods, "my functions for steganography\n");
}
#endif
