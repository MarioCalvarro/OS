#include <stdio.h>

#define N 10
#define MAXELEM 5000
#define MAXVALID 100

// function that prints on the standard output the contents of the array of v of size size
void imprimeArray(int v[],int size)
{
	int i;
	printf("-------------------\n");
	for (i=0;i<size;i++)
		printf("%d ",v[i]);
	printf("\n\n");
}

// function that copies the contents of a source array in a destination array
void copyArray(int src[],int dst[],int size)
{
    for (int i = 0; i < N; i++) {
        dst[i] = src[i]; 
    }
}

void tmo()
{
	int x = -1;
	int a[4] = {0,1,2,3};
	int b = 10000;
	int c = -1;
	int i;

	for  (i = 4; i < 6; i++)
		a[i] = i;

	printf("x %d b %d c %d\n", x, b, c);
}

int main()
{
	int A[N] = {4,3,8,5,6,9,0,1,7,2};
	int B[N];

	tmo();
	copyArray(A,B,N);
	imprimeArray(B,N);
}
