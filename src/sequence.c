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
//#include <numpy/arrayobject.h>

#define MLS__DOC__ ""\
"mls(N,create='one'):\n" \
"N:      demension of monic irreducible polynomial\n" \
"create: generate 'one' or 'full' MLS\n" \
"Returns one Maximum Length Sequence \n"
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
			PyObject *mseq=PyList_New(N-1);
			int init=1;
			int lfsr=init&(N-1);
			f>>=1;
			count++;
			for(int k=0;k<N-1;k++){
				lfsr=(lfsr>>1)^(-(int)(lfsr&1) & f);
				//PyList_SET_ITEM(mseq,k,(lfsr&1)?Py_True:Py_False);
				PyList_SET_ITEM(mseq,k,(lfsr&1)?Py_BuildValue("i",1):Py_BuildValue("i",-1));
			}
			if(create[0]=='o') return Py_BuildValue("O", mseq);
			else PyList_Append((PyObject*)mseqs,(PyObject*)mseq);
		}
	}
	if(create[0]=='o') return NULL;
	else return (PyObject*)mseqs;
}

#define CCC__DOC__ ""\
"ccc(N,seed=123456):\n" \
"N:      CCC(N,N,N**2)\n" \
"seed:   random seed number\n" \
"Returns CCC three dimensional py-list \n"
PyObject *ccc(PyObject *self, PyObject *args, PyObject* keywds) {
	int seed=123456,N;
	static char* kwlist[] = {"N","seed",NULL};
	if(!PyArg_ParseTupleAndKeywords(args,keywds,"i|i",kwlist,&N,&seed)) return NULL;
	char ***CCC=generateCCC(seed,N);
	PyObject *ccc=PyList_New(N);
	for(int i=0;i<N;i++){
		PyObject *cc=PyList_New(N);
		for (int j=0;j<N;j++){
			PyObject *c=PyList_New(N*N);
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

#define GETBASESEQUENCE__DOC__ ""\
"getBaseSequence(CCC,datasize,shift,ch):\n" \
"CCC:      CCC(N,N,N**2)\n" \
"datasize: data size/data length [bit]\n" \
"shift:    shift size when you're gonna make embed sequence [tip]\n" \
"ch:       CCC chanel to use (1-N)\n" \
"Returns base sequence \n"
PyObject *getBaseSequence(PyObject **self, PyObject *args, PyObject* keywds){
	static char* kwlist[] = {"CCC","datasize","shift","ch",NULL};
	PyObject *ccc;
	int ch=1,shift,datasize;
	if(!PyArg_ParseTupleAndKeywords(args,keywds,"Oii|i",kwlist,&ccc,&datasize,&shift,&ch)) return NULL;
	size_t N=PyList_GET_SIZE(ccc);
	size_t NN=N*N;
	size_t zero=shift*(datasize-1);//ゼロ埋めの数
	//PyObject *s=PyList_New((int)(N*N*N+zero*(N-1)));//メモリコンフリクトする
	PyObject *s=PyList_New(0);
	unsigned i,j;
	for(i=0;i<N;i++){
		//char ss=(NN+zero)*i;
		PyObject *c=PyList_GET_ITEM(PyList_GET_ITEM(ccc,ch-1),i);
		for(j=0;j<NN;j++) PyList_Append(s,PyList_GET_ITEM(c,j));//PyList_SET_ITEM(s,ss+j,PyList_GET_ITEM(c,j));
		for(j=0;j<zero;j++) PyList_Append(s,Py_BuildValue("i",0));
		//for(j=NN;j<NN+zero;j++) PyList_SET_ITEM(s,ss+j,Py_BuildValue("i",0));
	}
	return s;
}
#define GETEMBEDSEQUENCE__DOC__ ""\
"getEmbedSequence(base,data,N,shift):\n" \
"base:   base sequence\n" \
"data:   bit list data such as [1,0,0,0,1,1]. [bit]\n" \
"N:      CCC size.\n" \
"shift:  the shift value when the base sequence is Convoluted. [tip]\n" \
"Returns embed sequence.\n"
PyObject *getEmbedSequence(PyObject **self, PyObject *args, PyObject* keywds){
	static char* kwlist[] = {"base","data","N","shift",NULL};
	PyObject *s,*d;
	unsigned shift,N;
	if(!PyArg_ParseTupleAndKeywords(args,keywds,"OOii",kwlist,&s,&d,&N,&shift)) return NULL;
	int datasize=PyList_GET_SIZE(d);
	unsigned zero=shift*(datasize-1);//ゼロ埋めの数
	unsigned n=N*N*N+zero*(N-1); //基礎系列sの長さ 最後のゼロ埋めは不要
	unsigned NN=N*N;
	int *y=(int *)calloc(n+zero,sizeof(int)); //埋め込み系列
	PyObject *yy=PyList_New((int)(n+zero)); //埋め込み系列 pylist
	unsigned i,j,k;
	for(i=0;i<datasize;i++){
		int *yit=y+i*shift;
		int di=PyInt_AS_LONG(PyList_GET_ITEM(d,i));
		for(j=0;j<N;j++){
			int jNz=j*(NN+zero);
			for(k=jNz;k<jNz+NN;k++) yit[k]+=di*PyInt_AS_LONG(PyList_GET_ITEM(s,k));
		}
	}
	for(i=0;i<n+zero;i++) PyList_SET_ITEM(yy,i,Py_BuildValue("i",y[i]));
	free(y);
	return yy;
	//return Py_BuildValue("i",1);
}


void initsequence(void){
	static PyMethodDef methods[]={
		{"mls", (PyCFunction)mls, METH_VARARGS|METH_KEYWORDS , MLS__DOC__},
		{"ccc", (PyCFunction)ccc, METH_VARARGS|METH_KEYWORDS , CCC__DOC__},
		{"getBaseSequence" , (PyCFunction)getBaseSequence , METH_VARARGS|METH_KEYWORDS , GETBASESEQUENCE__DOC__},
		{"getEmbedSequence", (PyCFunction)getEmbedSequence, METH_VARARGS|METH_KEYWORDS , GETEMBEDSEQUENCE__DOC__},
		{NULL, NULL} /* sentinel */
	};
	Py_InitModule3("sequence", methods, "my functions for steganography\n");
}
