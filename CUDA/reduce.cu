#include <stdio.h>
#include <math.h>

__global__ void evenReduce(int *a, int *b, int numP)
{
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index < numP)
		b[index] = a[index * 2] + a[index * 2 + 1];
}

__global__ void oddReduce(int *a, int *b, int numP)
{
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index < numP) 
	{
	if(index != numP - 1)
			{ 
				b[index] = a[index * 2] + a[index * 2 + 1];
			}
	else 
			{
				/*puts the remaining value that doesn't have a pair in the right index*/
				b[index] = a[index * 2];
			}
	}		
} 

#define LENGTH 512
#define BLOCK_THREADS 512

int main()
{
	double length = LENGTH;
	int numP, l;

	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
   	
   	/*create arrays for host and GPU*/
   	int *a, *b, *k_b, *k_a;
	int size = length * sizeof( int );

	a = (int *)malloc( size );
	b = (int *)malloc( size );

	cudaMalloc( (void **) &k_a, size );
	cudaMalloc( (void **) &k_b, size );

   	/*initialize the array*/
	for( int i = 0; i < length; i++ )
	{
		a[i] = i;
		b[i] = 0;
	}

	/*array debug*/
	// printf("A:\n");
	// for(int i=0; i< length; i++)
	// {
	// 	printf("%d ", a[i]);
	// }

	/* copy inputs to device */
	cudaMemcpy(k_a, a, size, cudaMemcpyHostToDevice );
	cudaMemcpy(k_b, b, size, cudaMemcpyHostToDevice );

 	dim3 dimGrid( 1, 1 );
 	dim3 dimBlock(BLOCK_THREADS, 1);
	
	/*Since each thread does 2 additions there are log2(N) iterations.*/
	int gates = ceil(log(length) / log(2));
	
	cudaEventRecord(start);
	
	for(int i=0; i < gates; i++) {

		/*get the number of threads needed. Ceiling used for odd array lengths*/
		numP = ceil(length/2);
				
		l = (int)length;
		/*when threads are divisble by 2 use less code...*/
		if( l % 2 == 0)
			evenReduce<<<dimGrid,dimBlock>>>(k_a, k_b, numP);
		else 
			oddReduce<<<dimGrid,dimBlock>>>(k_a, k_b, numP);
		
		/*last # of threads will equal next array length to compute*/
		length = numP;
		
		// printf("\niteration %d reduction is\n", i + 1);
		// cudaMemcpy(b, k_b, size, cudaMemcpyDeviceToHost );
		// for(int i=0; i< N; i++)
		// 	{
		// 	printf("%d ", b[i]);
		// 	}

		/*send array b's data back to a after each iteration*/
		cudaMemcpy(k_a, k_b, size, cudaMemcpyDeviceToDevice );
	}
	
	cudaEventRecord(stop);
	cudaEventSynchronize(stop);
	float milliseconds = 0;
	cudaEventElapsedTime(&milliseconds, start, stop);

	/* copy result back to host*/

	cudaMemcpy(b, k_b, size, cudaMemcpyDeviceToHost );
	printf("\nFinal reduction is %d\n", b[0]);
	printf("\nThis took %f milliseconds\n", milliseconds);

	/* clean up */

	free(a);
	free(b);
	cudaFree( k_a );
	cudaFree( k_b );
	
	return 0;
} 

