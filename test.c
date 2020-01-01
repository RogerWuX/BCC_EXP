#include<malloc.h>
#include<unistd.h>
#include<time.h>
#include<stdlib.h>



int main()
{
	clock_t start, end;
	sleep(30);
	start = clock();
	int size=32768;
	int* m=(int*)malloc(size*sizeof(int));
	for(int j=0;j<4;++j)
	{
		for(int i=j*size/4;i<size/4;++i)
		{
			m[i]=i;
		}
		sleep(1);
	}
	end = clock();
	printf("%ld msec",end-start);
	
}
