/* CCC(N,N,N^2)生成ライブラリ 5J13佐田悠生 2015.11.2 */
#include<stdio.h>
#include<stdlib.h>

/* 変数の1であるbitの数を調べる */
int popcount(int bits){
    bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
    bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
    bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
    bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
    return (bits & 0x0000ffff) + (bits >>16 & 0x0000ffff);
}
/* CCC(N,N,N^2)を生成する {{{
 * seed:   アダマール行列生成用の乱数シード値
 * 戻り値: ccc系列N^4 1次元配列 }}}*/
int ***generateCCC(int seed,unsigned N){
	char **H;
	unsigned i,j,k,l;
	int r[N],tr=0;//rの各要素はランダムな+-1 pdfにある対角成分(対角行列X)に対応する
	int ***CCC;
	unsigned NNN=N*N*N,NN=N*N;
	if(N&(N-1)){
		fprintf(stderr,"ERROR: deta size is not the power of 2.\n");
		exit(-1);
	}

	// メモリ確保
	H=(char**)malloc(sizeof(char*)*N);
	*H=(char*)malloc(sizeof(char)*N*N);
	CCC=(int***)malloc(sizeof(int**)*N);
	*CCC=(int**)malloc(sizeof(int*)*N*N);
	**CCC=(int*)malloc(sizeof(int)*N*N*N*N);
	for(i=0;i<N;i++){
		CCC[i]=*CCC+i*N;
		H[i]=*H+i*N;
		for(j=0;j<N;j++) CCC[i][j]=**CCC+i*NNN+j*NN;
	}

	//対角成分
	srand(seed);
	for(i=0;i<N;i++){
		r[i]=rand()%2*2-1;
		tr+=r[i];
	}
	if((unsigned)tr==N || tr==-(int)N){//乱数が全て1もしくは全て-1になり等価なアダマール行列が生成されちゃう
		r[0]=-r[0];
	}
	//アダマール行列生成
	for(i=0;i<N;i++)
		for(j=i;j<N;j++){
			H[i][j]=1-popcount(i&j)%2*2;
			H[j][i]=H[i][j]; //対称行列
		}
	/* HからCCCを作成 */
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			char *Hj=H[j];
			for(k=0;k<N;k++){
				int a=r[k]*H[i][k]*r[j];
				int *c=CCC[i][j]+N*k;
				for(l=0;l<N;l++) c[l]=a*H[k][l]*Hj[l];
			}
		}
	}
	return CCC;
}
/* 基礎系列生成 {{{
 * ccc,nsize:CCC(nsize,nsize,nsize^2)のccc系列
 * ch:使用するチャネル1以上nsize以下
 * t :shit数
 * dsize:埋め込みビット数 [bit]
 * 戻り値:基礎系列 }}}*/
int *generateBaseSequence(int ***ccc,unsigned ch,unsigned t,unsigned dsize,unsigned nsize){
	unsigned square=nsize*nsize;
	unsigned zero=t*(dsize-1);//ゼロ埋めの数
	int  *s=(int *)malloc((nsize*nsize*nsize+zero*(nsize-1))*sizeof(int)); //基礎系列 callocで0クリア
	unsigned i,j;
	for(i=0;i<nsize;i++){
		int *ss=s+(square+zero)*i;
		int *c=ccc[ch-1][i];
		for(j=0;j<square;j++) ss[j]=c[j];
		if(i!=nsize-1) for(j=square;j<square+zero;j++) s[j]=0;
	}
	return s;
}
/* 埋め込み系列生成 {{{
 * s :基礎系列
 * d :埋め込みデータ
 * t :shit数
 * dsize:配列dの長さ
 * nsize:CCC(nsize,nsize,nsize^2)のccc系列
 * 戻り値:埋め込み系列 }}}*/
int *generateEmbedSequence(int s[],int d[],unsigned t,unsigned dsize,unsigned nsize){
	unsigned zero=t*(dsize-1);//ゼロ埋めの数
	size_t n=nsize*nsize*nsize+zero*(nsize-1); //基礎系列sの長さ 最後のゼロ埋めは不要
	unsigned NN=nsize*nsize;
	int *y=(int*)malloc((n+zero)*sizeof(int)); //埋め込み系列
	unsigned i,j,k;
	for(i=0;i<dsize;i++){
		int *yit=y+i*t;
		int di=d[i];
		for(j=0;j<nsize;j++){
			int jNz=j*(NN+zero);
			for(k=jNz;k<jNz+NN;k++) yit[k]+=di*s[k];
		}
		for(j=NN*i;j<i*t;j++) y[j]=0;
	}
	return y;
}

