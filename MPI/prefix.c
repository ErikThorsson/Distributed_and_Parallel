#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>


int local_sum(int *ar, int start, int end, int myid) {
    int sum = 0, j;
    for(j=start; j<=end; j++)
    {
      sum+=ar[j];
    }
   printf("local sum of partition %d is %d\n", myid, sum);
   return sum;
}

int main(argc,argv)
int argc;
char *argv[];
{
    MPI_Status status;
    int ar[10], r,n =10,numprocs, myid,i,j,sum,localSum,localPre,newSum,sum2,start, end, part;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs); //# processes
    MPI_Comm_rank(MPI_COMM_WORLD,&myid); //id

  srand(time(NULL));
  for(i=0;i<n;i++) { //create an array of random 1s and 0s
    r = rand() % 2;
    ar[i] = r;
    MPI_Bcast(ar, n, MPI_INT, 0, MPI_COMM_WORLD);
    }

if(myid == 0) { //print random array 
  for(i=0;i<n;i++) 
  {
   printf("%d ", ar[i]);
 }
    printf("\n");
  }

  part = n/numprocs; //get size of partitions
  start = myid * part;
  end = start + part -1;

  if(myid == numprocs - 1) //adjust last partition if there is a remainder
  {
      end = n -1;
  }
 
    sum = local_sum(ar, start, end, myid);
    localSum = sum;
    
    //send your local sum to every process ahead of you
    for(i=myid; i < numprocs - 1; i++)
    {
    MPI_Send(&sum, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    newSum = 0; //for each partition before, add their local sums to get this one's prefix
    for(i=myid; i > 0; i--)
    {
    MPI_Recv(&sum, 1, MPI_INT, i-1, 0, MPI_COMM_WORLD, &status); 
    newSum+=sum;
    }

    printf("process %d has a prefix of %d\n", myid, newSum + localSum);

    MPI_Barrier(MPI_COMM_WORLD);

    //now get the prefixes within each chunk
    sum2 = 0;
    int counter = 0, pres[end-start +1];
    for(j=start; j<=end; j++)
    {
      pres[counter] = ar[j] + sum2;
      sum2 += ar[j];
      counter++;
    }
     for(i = 0; i< end-start +1 ;i++)
        printf("%d ", pres[i]);
    printf("\n");

    //sum = local_sum(ar, start, end, myid);
      //like in part 2, send your chunk prefix array to processes in front
    // for(i=myid; i < numprocs - 1; i++)
    // {
    // MPI_Send(&sum, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
    // }

    // MPI_Barrier(MPI_COMM_WORLD);

    // for(i=myid; i > 0; i--)
    // {
    // MPI_Recv(&sum, 1, MPI_INT, i-1, 0, MPI_COMM_WORLD, &status); 
    // //add the localSum to each if not p0
    //  for(j = 0; j< end-start +1 ;j++) 
    //  {
    //     pres[j]+=sum;
    //   }
    // }
    //   printf("\n");
    //   for(i = 0; i< end-start +1 ;i++)
    //     printf("%d ", pres[i]);

    //send arrays to p0 to combine
    // if(myid != 0) {
    // MPI_Send(&pres, end - start + 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    // }

    // if(myid == 0)
    // {
    // int final[n];
    // for(i=0; i<part;i++) //copy p0 to final array
    //   final[i] = pres[i];

    // for(i=1; i < numprocs; i++)
    // {
    // if(i != numprocs-1)
    //   MPI_Recv(&pres, part, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); 
    // else 
    //    MPI_Recv(&pres, n - (numprocs -1) * part + 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); 
    //     //add array to master final array 
    //     if(i != numprocs -1) {
    //     for(j=0;j<part;j++)
    //        final[i * part + j] = pres[j];
    //     } else {
    //        for(j=0;j< n - (numprocs -1) * part + 1;j++)
    //        final[i * part + j] = pres[j];
    //     }
    // }

    // //print final array
    //   printf("\n");
    //   for(i = 0; i< n;i++)
    //     printf("%d ", final[i]);
    //}
    MPI_Finalize();
    return 0;
 }

