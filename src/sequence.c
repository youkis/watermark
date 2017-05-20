#include <Python.h>
#include <strings.h>
#include <stdlib.h>
#include "ccc.h"
//#include <numpy/arrayobject.h>

PyObject *mls(PyObject *self, PyObject *args, PyObject* keywds) {
	int n;
  char* create="one";
	int count=0;
	static char* kwlist[] = {"N","create",NULL};
	if(!PyArg_ParseTupleAndKeywords(args,keywds,"i|s",kwlist,&n,&create)) return NULL;
	if(strcmp(create,"one")!=0 && strcmp(create,"full")!=0){
		fprintf(stderr,"second arg has to be 'one' or 'two'");
		return NULL;
	}
	int N=1<<n;
	PyObject *mseq=PyList_New(N-1);
	PyObject *mseqs=PyList_New(0);

	// 原始多項式
	for(int i=1;i<N;i+=2){ // 原始多項式fの最下位ビットは必ず1なので2ずつインクリメント
		int f=N^i;
		int a=N;
		int j;
		for(j=n;j<N;j++){
			if((a&N)==N) a^=f;
			if(a==1) break;
			a<<=1;
		}
		if(j==N-1){ // fが原始多項式であるからfからM系列を求める
			int init=1;
			int lfsr=init&(N-1);
			f>>=1;
			count++;
			for(int k=0;k<N-1;k++){
				lfsr=(lfsr>>1)^(-(int)(lfsr&1) & f);
				PyList_SET_ITEM(mseq,k,(lfsr&1)?Py_True:Py_False);
			}
			if(create[0]=='o') return Py_BuildValue("O", mseq);
			else PyList_Append((PyObject*)mseqs,(PyObject*)mseq);
		}
	}
	if(create[0]=='o') return NULL;
	else return (PyObject*)mseqs;
}

PyObject *ccc(PyObject *self, PyObject *args, PyObject* keywds) {
	int seed=123456,N;
	static char* kwlist[] = {"N","seed",NULL};
	if(!PyArg_ParseTupleAndKeywords(args,keywds,"i|i",kwlist,&N,&seed)) return NULL;
	char ***CCC=generateCCC(seed,N);
	PyObject *c=PyList_New(N*N),*cc=PyList_New(N),*ccc=PyList_New(N);
	for(int i=0;i<N;i++){
		for (int j=0;j<N;j++){
			for (int k=0;k<N*N;k++) PyList_SET_ITEM(c,k,Py_BuildValue("i",CCC[i][j][k]));
			PyList_SET_ITEM(cc,j,c);
		}
		PyList_SET_ITEM(ccc,i,cc);
	}
	free(**CCC);
	free(*CCC);
	free(CCC);
	return ccc;
}
//PyObject *correlate(PyObject *self, PyObject *args){
//	PyObject *a,*b; /* the list of strings */
//	if (! PyArg_ParseTuple( args, "OO",&a,&b)) return NULL;
//	a=Py_BuildValue("O",a);
//	//return Py_BuildValue("i",*a->data);
//	return PyList_GET_ITEM(a,0);
//}

#define MLS__DOC__ ""\
"mls(N,create='one'):\n" \
"N:      demension of monic irreducible polynomial\n" \
"create: generate 'one' or 'full' MLS\n" \
"Returns one Maximum Length Sequence \n"
#define CCC__DOC__ ""\
"ccc(N,seed=123456):\n" \
"N:      CCC(N,N,N**2)\n" \
"seed:   random seed number\n" \
"Returns CCC three dimensional py-list \n"

void initsequence(void){
	static PyMethodDef methods[]={
		{"mls", (PyCFunction)mls, METH_VARARGS|METH_KEYWORDS , MLS__DOC__},
		{"ccc", (PyCFunction)ccc, METH_VARARGS|METH_KEYWORDS , CCC__DOC__},
		{NULL, NULL} /* sentinel */
	};
	Py_InitModule3("sequence", methods, "my functions for steganography\n");
}
