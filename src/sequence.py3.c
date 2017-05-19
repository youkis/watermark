#include <Python.h>

PyObject *msequence(PyObject *self, PyObject *args){
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
				if(lfsr&1) PyList_SET_ITEM(mseq,k,Py_True);
				else       PyList_SET_ITEM(mseq,k,Py_False);
			}
			return Py_BuildValue("O", mseq);
			//PyList_Append((PyObject*)mseqs,(PyObject*)mseq);
		}
	}
	return NULL;
	//return (PyObject*)mseqs;
}

PyObject *PyInit_myfunction(void){
	static PyMethodDef methods[] = {
		{"msequence", msequence, METH_VARARGS, "return several M sequences.\n"},
		{NULL, NULL}
	};
	PyDoc_STRVAR(api_doc, "myfunction module.\n");
	static struct PyModuleDef cmodule = {
		PyModuleDef_HEAD_INIT,
		"myfunction",     /* name of module */
		api_doc, /* module documentation, may be NULL */
		-1,      /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
		methods
	};
	return PyModule_Create(&cmodule);
}
