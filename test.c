#include<malloc.h>
#include<unistd.h>
#include<time.h>
#include<stdlib.h>

#define array_size 1024

int main()
{
	int stage=0;
	char buf[64];
	fscanf(stdin,"%s",buf);
	printf("%d\n",stage++);
	fscanf(stdin,"%s",buf);
	printf("%d\n",stage++);
	clock_t start, end;
	int* m=(int*)malloc(array_size*sizeof(int));	
	fscanf(stdin,"%s",buf);
	printf("%d\n",stage++);
	start = clock();
	for(int i=0;i<array_size;++i)
	{
		m[i]=i;
	}
	end = clock();
	printf("execution time %ld msec\n",end-start);
	
}
