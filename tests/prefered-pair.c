#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int innerPro(char *x,char *y,size_t n,int t){
	int ret=0;
	for(int i=0;i<n;i++)ret+=x[i]*y[(i+t)%n];
	return ret;
}

int main(){
	int N=7;
	int p=pow(2,N);
	int i,j,k;
	char *m[1000];
	unsigned count=0;
	printf("N=%d\n",N);
	printf("size=%d\n\n",p);
	// 原始多項式
	for(i=1;i<p;i+=2){ // 原始多項式fの最下位ビットは必ず1なので2ずつインクリメント
		int f=p^i;
		int a=p;
		for(j=N;j<p;j++){
			if((a&p)==p) a^=f;
			if(a==1) break;
			a<<=1;
		}
		if(j==p-1){ // fが原始多項式であるからfからM系列を求める
			int init=1;
			int lfsr=init&(p-1);
			m[count]=(char *)malloc(sizeof(char)*(p-1));
			f>>=1;
			for(k=0;k<p-1;k++){
				lfsr=(lfsr>>1)^(-(int)(lfsr&1) & f);
				m[count][k]=(lfsr&1)*2-1;
			}
			count++;
		}
	}
	for(i=0;i<count;i++){
		printf("No%2d:",i+1);
		for(j=0;j<p-1;j++) printf("%2d,",m[i][j]);
		printf("\n");
	}
	int pre[]={-1+pow(2,(int)((N+2)/2)),-1-pow(2,(int)((N+2)/2)),-1};
	printf("プリファードペアな系列の組\n");
	for(i=0;i<count;i++)
		for(j=i+1;j<count;j++){
			char flag=1;
			for(k=0;k<p-1;k++){
				int inn=innerPro(m[i],m[j],p-1,k);
				if(inn!=pre[0] && inn!=pre[1] && inn!=pre[2]){
					flag=0;
					break;
				}
			}
			if(flag) printf("No:%2d,%2d\n",i+1,j+1);
		}
	return 0;
}

