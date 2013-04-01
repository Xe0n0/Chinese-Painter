#define max 33 
#define scale 8
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
int a[max+1][max+1];
void reset1(int x,int y,int step,int seed)
{
	srand(time(0));
	int sum=0;
	int point=0;
	int tmp[]={x+step,x-step,y+step,y-step};
	if(tmp[0]<=max){sum+=a[tmp[0]][y];point++;}
	if(tmp[3]>=0){sum+=a[x][tmp[3]];point++;}
	if(tmp[1]>=0){sum+=a[tmp[1]][y];point++;}
	if(tmp[2]>=max){sum+=a[x][tmp[2]];point++;}
	a[x][y]+=sum/point;
	if(seed!=0)a[x][y]+=rand()%(2*seed)-seed;
}
int dsquare(int x1,int y1,int x2,int y2,int seed)
{
	if(x1+1==x2)return 0;
	srand(time(0));
	int midx=(x1+x2)/2;
	int midy=(y1+y2)/2;
	a[midx][midy]+=(a[x1][y1]+a[x1][y2]+a[x2][y1]+a[x2][y2])/4;
	if(seed!=0)
	a[midx][midy]+=rand()%(2*seed)-seed;
	//seed/=2;
	reset1(midx,y1,midx-x1,seed);
	reset1(x1,midy,midx-x1,seed);
	reset1(x2,midy,midx-x1,seed);
	reset1(midx,y2,midx-x1,seed);
	seed/=2;
	dsquare(x1,y1,midx,midy,seed);
	dsquare(midx,y1,x2,midy,seed);
	dsquare(x1,midy,midx,y2,seed);
	dsquare(midx,midy,x2,y2,seed);
	return 0;
}

int main()
{
	dsquare(1,1,max,max,scale);
	int i,j;
	for(i=1;i<=max;i++)
	{
		for(j=1;j<=max;j++)
		printf("%d ",a[i][j]);
		printf("\n");
		
		}
		return 0;
}
