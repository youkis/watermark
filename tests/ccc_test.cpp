/* CCCを使った埋め込み抽出シミュレーションプログラム
 * 2015.11.4 */
#include<iostream>
#include"../src/ccc.h"
#include<fstream>
#include<math.h>

#define N      32 //CCC(N,N,N*N)を使って埋め込み
#define DataSize 50 //埋め込むビット数
#define T       1 //shift数
#define ALPHA   1 //埋め込み強度
#define U   255.0/2 // 正規化乱数平均
#define S      10.0 // 正規化乱数標準偏差

using namespace std;

int innerPro(int *x,int *Si,size_t n){
	int ret=0;
	for(int i=0;i<n;i++)ret+=x[i]*Si[i];
	return ret;
}
int dice(){//+-1生成器
	return rand()%2*2-1;
}
int gauss(){
	unsigned rnd;
	while(!(rnd=rand()));
	return 0.5+U+S*sqrt(-2.0*log((double)rnd/RAND_MAX))*cos(2.0*M_PI*rand()/RAND_MAX);
}
double dispersion(int *x,size_t n){
	int sum=0;
	double ret=0;
	for(int i=0;i<n;i++)sum+=x[i];
	//cout<<sum/n<<"\n";
	for(int i=0;i<n;i++){
		double mean=(double)sum/n;
		ret+=(x[i]-mean)*(x[i]-mean);
	}
	return ret/n;
}
int main(){
	size_t zero=T*(DataSize-1);//ゼロ埋めの数
	size_t n=N*N*N+zero*(N-1);                      //基礎系列sの長さ 最後のゼロ埋めは不要
	int ***CCC=generateCCC(123456,N),
	        *s=generateBaseSequence(CCC,1,T,DataSize,N),//基礎系列 callocで0クリア
	        *d=(int *)malloc(sizeof(int)*DataSize); //埋め込むデータ
	for(int i=0;i<n;i++) printf("%2d,",s[i]);       cout<<"\b \n";
	//埋め込みデータを乱数生成
	srand((unsigned)time(NULL));
	for(int i=0;i<DataSize;i++) d[i]=dice();
	int *y=generateEmbedSequence(s,d,T,DataSize,N);
	ofstream ofs("a.txt"); for(int i=0;i<n+zero;i++) ofs<<y[i]<<"\n"; //埋め込み系列書き出し
	for(int i=0;i<n+zero;i++) printf("%2d,",y[i]);       cout<<"\b \n";
	//ガウス雑音付加(画素値)
	srand((unsigned)time(NULL));
	for(int i=0;i<n+zero;i++){
		int r=gauss();
		y[i]=r+ALPHA*y[i];
		//y[i]=r+r*ALPHA*y[i];
	}
	//抽出
	cout<<"埋込データ:";
	for(int i=0;i<DataSize;i++) cout<<(int)(d[i]+1)/2<<","; cout<<"\b \n";
	cout<<"抽出データ:";
	//平均を引く
	double sum=0;
	for(int i=0;i<n+zero;i++)sum+=y[i];
	sum/=n+zero;
	for(int i=0;i<n+zero;i++)y[i]-=(int)(sum);
	//相関を取る
	ofstream soukan("soukan.txt");
	for(int i=0;i<DataSize;i++){
		int *p=y+i*T; //shift数1でずらして内積を取っていく
		cout<<(~(unsigned)innerPro(p,s,n)>>(sizeof(int)*8-1))<<",";
		soukan<<innerPro(p,s,n)<<"\n";
	}
	cout<<"\b \n";
	free(**CCC);
	free(*CCC);
	free(CCC);
	return 0;
}

