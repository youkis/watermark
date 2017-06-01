#include"../src/mls.h"

int main(){
	unsigned pref_size;
	unsigned n=7,N=1<<n;
	unsigned i,j;
	char **prefm=preferd(n,&pref_size);
	for(i=0;i<pref_size;i++){
		printf("%2d: ",i);
		for(j=0;j<N-1;j++) printf("%d,",prefm[i][j]);
		printf("\n");
	}
	free(prefm[0]);
	free(prefm);
	return 0;
}
