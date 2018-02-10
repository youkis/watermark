#include"../src/mls.h"

int main(){
	unsigned mls_size;
	unsigned n=5,N=1<<n;
	unsigned i,j;
	//int **m=mls(n,&mls_size,1);
	int **m=preferd(n,&mls_size);
	printf("size:%d\n",mls_size);
	for(j=0;j<N-1;j++) printf("%d,",m[0][j]);
	printf("\n");
	printf("\n");
	for(j=0;j<N-1;j++) printf("%d,",innerPro(m[0],m[1],N-1,j));
	free(*m);
	free(m);
	return 0;
}
