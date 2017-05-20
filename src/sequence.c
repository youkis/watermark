#include <Python.h>
#include "ccc.h"
//#include <numpy/arrayobject.h>

PyObject *mls(PyObject *self, PyObject *args){
	int n;
	int count=0;
	if(!PyArg_ParseTuple(args,"i",&n)) return NULL;
	int N=1<<n;
	PyObject *mseq=PyList_New(N-1);
	//PyObject *mseqs=PyList_New(0);

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
			return Py_BuildValue("O", mseq);
			//PyList_Append((PyObject*)mseqs,(PyObject*)mseq);
		}
	}
	return NULL;
	//return (PyObject*)mseqs;
}

PyObject *ccc(PyObject *self, PyObject *args, PyObject* keywds) {
	int seed=123456,N;
  PyObject* eList = NULL;
	static char* kwlist[] = {"N","seed",NULL};
	if(!PyArg_ParseTupleAndKeywords(args,keywds,"i|i",kwlist,&N,&eList,&seed)) return NULL;
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

#define CCC__DOC__ ""\
"mls(N,seed=123456):\n" \
"N:      CCC(N,N,N**2)\n" \
"seed:   random seed number\n" \
"Returns CCC three dimension python list \n"

void initsequence(void){
	static PyMethodDef methods[]={
		{"mls", mls, METH_VARARGS, "return several M sequences.\n"},
		{"ccc", (PyCFunction)ccc, METH_VARARGS|METH_KEYWORDS, CCC__DOC__},
		{NULL, NULL} /* sentinel */
	};
	Py_InitModule3("sequence", methods, "my functions for steganography\n");
}
