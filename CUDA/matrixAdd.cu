#include <stdio.h>

__global__ void vector_add(int *a, int *b, int *c)
{
    /* insert code to calculate the index properly using blockIdx.x, blockDim.x, threadIdx.x */
	int index = threadIdx.x;
	c[index] = a[index] + b[index];
}

#define dim 3

int main()
{
    int *a, *b, *c;
	int *d_a, *d_b, *d_c;
	//since we will be sending an array to represend the matrix.
	int size = dim * dim * sizeof( int );

	/* allocate space for device copies of a, b, c */

	cudaMalloc( (void **) &d_a, size );
	cudaMalloc( (void **) &d_b, size );
	cudaMalloc( (void **) &d_c, size );

	/* allocate space for host copies of a, b, c and setup input values */

	a = (int *)malloc( size );
	b = (int *)malloc( size );
	c = (int *)malloc( size );

	for( int i = 0; i < dim * dim; i++ )
	{
		a[i] = b[i] = i;
		c[i] = 0;
	}

	printf("A and B are:\n");
	for(int i=0; i< dim * dim; i++)
	{
		if(i%dim == 0)
			printf("\n");
		printf("%d ", a[i]);
	}

	/* copy inputs to device */
	cudaMemcpy( d_a, a, size, cudaMemcpyHostToDevice );
	cudaMemcpy( d_b, b, size, cudaMemcpyHostToDevice );

	dim3 dimBlock(10, 1 );
 	dim3 dimGrid( 1, 1 );
	
	vector_add<<<dimGrid,dimBlock>>>( d_a, d_b, d_c);

	/* copy result back to host */
	cudaMemcpy( c, d_c, size, cudaMemcpyDeviceToHost );

	printf("\n\nTheir sum =\n");
	for(int i=0; i< dim * dim; i++)
	{
		if(i%dim == 0)
			printf("\n");
		printf("%d ", c[i]);
	}

	free(a);
	free(b);
	free(c);
	cudaFree( d_a );
	cudaFree( d_b );
	cudaFree( d_c );
	
	return 0;
} 

