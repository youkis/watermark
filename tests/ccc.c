/* CCC(N,N,N^2)生成プログラム 5J13佐田悠生 2015.11.2
 * 高速化のため配列は全て１次元配列を使用
 * array[i][j]はarray[i*N+j]と同義
 * 大きな配列はchar型を使用。二値なので十分。 */
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"../src/ccc.h"

#define SIZE 2
#define SEED 123456

int main(int argc,char **argv){
	int i,j,k,
	    N   =argc>=2?atoi(argv[1]):SIZE,
	    seed=argc>=3?atoi(argv[2]):SEED;
	if(argc==2 && argv[1][1]=='h'){
		printf("usage1: [data size N] [seed number]\nusage2: [data size N]\n");
		return 0;
	}
	char ***CCC=generateCCC(seed,N);
	printf("seed number:%d\n",seed);
	for(i=0;i<N;i++)
	for(k=0;k<N;k++){
		for(j=0;j<N*N;j++)printf("%2d,",CCC[i][k][j]);
		printf("\b  \n");
	}
	free(**CCC);
	free(*CCC);
	free(CCC);
	return 0;
}

