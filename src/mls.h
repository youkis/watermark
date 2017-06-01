#include<stdio.h>
#include<stdlib.h>

int innerPro(char *x,char *y,size_t n,int t){
	int ret=0;
	for(int i=0;i<n;i++)ret+=x[i]*y[(i+t)%n];
	return ret;
}
// GF(2)多項式の掛け算
unsigned mul(unsigned a,unsigned b){
	unsigned ret=0;
	while(a!=0){
		if(a&1) ret^=b;
		a>>=1;
		b<<=1;
	}
	return ret;
}
// m系列生成
// 戻り値 1つまたは複数のm系列 系列長 (1<<n)-1
// n: 次数
// mls_size: 算出したmlsの個数を返す
// is_full: 1で全てのmlsを算出 0で1本のみ
char **mls(unsigned n,unsigned *mls_size,char is_full){
	unsigned i,j,k;
	unsigned N2=1<<(n+1);  // N2未満の既約多項式を求める
	unsigned N=1<<n;
	char *flag=(char *)malloc(sizeof(char)*N2);
	unsigned irr_num=0;//既約多項式の数(数学的にはメビウス関数を用いて表せる)
	unsigned mls_num=0;//m系列の数
	unsigned *irr;     //既約多項式配列
	char **mseqes; //m系列用二次元配列
	char *mp; //2次元配列獲得用pointer

	// 既約多項式の配列irrを篩アルゴリズムで作成
	for(i=1;i<N2;i++) flag[i]=1;  // 全ての数に1を立てる
	flag[1]=0;                    // 1は既約ではない
	for(i=2;mul(i,i)<N2;i++)
		if(flag[i]==1)
			for(j=i;mul(i,j)<N2;j++) flag[mul(i,j)]=0;
	for(i=N+1;i<N2;i++) if(flag[i]) irr_num++;

	irr=malloc(sizeof(unsigned)*irr_num);
	irr_num=0;
	for(i=N+1;i<N2;i++) if(flag[i]) irr[irr_num++]=i;
	free(flag);

	// 原始多項式でないirr要素は省いてirrを再構築
	for(i=0;i<irr_num;i++){
		unsigned f=irr[i];
		unsigned a=N;
		for(j=n;j<N;j++){
			if((a&N)==N) a^=f; // N以上になったらa=a mod N
			if(a==1) break;
			a<<=1;
		}
		if(j<N-1)continue; // 周期が最大じゃなきゃ原始多項式じゃない
		irr[mls_num++]=f;
		if(!is_full) break;
	}

	//原始多項式からM系列生成
	mp=(char*)malloc(sizeof(char)*mls_num*(N-1));
	mseqes=(char**)malloc(sizeof(char*)*mls_num);
	for(i=0;i<mls_num;i++){// fが原始多項式であるからfからM系列を求める
		unsigned f=irr[i];
		unsigned lfsr=1;
		mseqes[i]=mp+i*(N-1); //mpにpointing
		f>>=1;
		for(k=0;k<N-1;k++){
			lfsr=(lfsr>>1)^(-(int)(lfsr&1) & f);
			mseqes[i][k]=(lfsr&1)*2-1;
		}
	}
	free(irr);
	//free(mseqes[0]);
	//free(mseqes);
	*mls_size=mls_num;
	return mseqes;
}

char **preferd(unsigned n, unsigned *pref_size){
	unsigned N=1<<n;
	int i,j,k;
	unsigned mls_size=0;
	char **m=mls(n,&mls_size,1);
	int mini_val1=-1;
	int mini_val2=-1+(1<<((n+2)/2)); //プリファードペアが取るべき3つの小さい値
	int mini_val3=-1-(1<<((n+2)/2));
	char mat[mls_size][mls_size];
	unsigned pair[mls_size+1],depth=0,pdepth=1;
	char **prefm,*prefmp;

	// 行列作成 要素が1である添字同士がプリファードペア
	for(i=0;i<mls_size;i++){
		mat[i][i]=0;
		for(j=i+1;j<mls_size;j++){
			char flag=1;
			for(k=0;k<N-1;k++){
				int inn=innerPro(m[i],m[j],N-1,k);
				if(inn!=mini_val1 && inn!=mini_val2 && inn!=mini_val3){
					flag=0;
					break;
				}
			}
			mat[i][j]=flag;
			mat[j][i]=flag;
		}
	}
	// 行列探索
	pair[depth]=0;
	while(pdepth!=depth){
		pdepth=depth;
		for(i=pair[depth]+1;i<mls_size;i++){
			if(mat[pair[depth]][i]){
				int flag=1;
				for(j=depth-1;j>=0;j--)
					if(mat[pair[j]][i]!=1){
						flag=0;
						break;
					}
				if(flag) pair[++depth]=i;
			}
		}
	}
	*pref_size=depth+1;
	prefm=(char**)malloc(sizeof(char*)*(*pref_size));
	prefmp=(char*)malloc(sizeof(char)*(*pref_size)*(N-1));

	for(i=0;i<depth+1;i++){
		prefm[i]=prefmp+i*(N-1);
		for(j=0;j<N-1;j++) prefm[i][j]=m[pair[i]][j];
	}

	free(m[0]);
	free(m);
	return prefm;
}

