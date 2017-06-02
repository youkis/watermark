#include"../src/mls.h"

int main(){
	unsigned mls_size;
	unsigned n=7,N=1<<n;
	unsigned i,j;
	char **m=mls(n,&mls_size,1);
	for(i=0;i<mls_size;i++){
		printf("%2d: ",i);
		for(j=0;j<N-1;j++) printf("%d,",innerPro(m[i],m[i],N-1,j));
		printf("\n");
	}
	free(*m);
	free(m);
	return 0;
}
