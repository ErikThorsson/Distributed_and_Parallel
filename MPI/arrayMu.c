#include "mpi.h"
#include <stdio.h>
#include <math.h>
//#define dimension 3;

int main(argc,argv)
int argc;
char *argv[];
{
    double elapsed_time;    
    MPI_Status status;
    int i,j,k,curCol, next, last, sum0, myid, numprocs, dimension = 10;
    int A[dimension][dimension], C[dimension][dimension], D[dimension];//D is the column for each thread
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs); //# processes
    MPI_Comm_rank(MPI_COMM_WORLD,&myid); //id

   /* start the timer */
    elapsed_time = -MPI_Wtime();

    //create array A
    for(i=0; i<dimension; i++) {
        for(j=0; j<dimension; j++) {
            A[i][j] = j + 1;
    }
}
//debug print array
if(myid ==0) {
    printf("Arrays A and B are:\n");

    for (i = 0; i < dimension; i++) {
        for (j = 0; j < dimension; j++) {
            printf("%d ", A[i][j]);
            }
         printf("\n");
        }
}
    //id0 needs to send the columns to the other threads
    if(myid == 0)
    {
        for(i =1; i<dimension;i++) {
            for(j=0; j<dimension;j++)
                D[j] = A[j][i]; 
                //now send the column to its thread
                 MPI_Send(&D, dimension, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
         for(j=0; j<dimension;j++) //make p0s column
                D[j] = A[j][0];
    }

    //now multiply each row by its column #. Each of these will be made into a thread! 
    //will loop for each dimension
    for(i=0;i<dimension;i++) {

            if(i == 0)
                curCol = myid;
            else
                curCol--; //keep track of column
            if(curCol < 0) 
                curCol = dimension-1;

            //get previous column process id
            if(myid - 1 >= 0) //if first iteration last is myid-1 if not neg num
                last = myid- 1;
            else 
                last = dimension - 1;

        if(i == 0 & myid != 0) //if first iteration all receieve from p0 but p0
            MPI_Recv(&D, dimension, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); 
        else if(i != 0) //else get column from last process
            MPI_Recv(&D, dimension, MPI_INT, last, 0, MPI_COMM_WORLD, &status); 
        
        MPI_Barrier(MPI_COMM_WORLD);

        sum0 = 0;
         for(j=0;j<dimension;j++) { 
            
            sum0 += A[myid][j] * D[j]; //multiply the current row by the first column. Will always only multiply the first column
            if(j == dimension - 1) {
                C[myid][curCol] = sum0;
                //printf("C[%d][%d] is %d\n",myid,curCol, sum0);
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

           // if(myid == 0)
           //      printf("------------------iteration %d---------------\n", i);

            if(myid + 1 <dimension) 
                next = myid+1;
            else 
                next = 0;
            
            //the matrix will then be translated right
            //if(i != dimension -1)
                MPI_Send(&D, dimension, MPI_INT,next, 0, MPI_COMM_WORLD); 

         MPI_Barrier(MPI_COMM_WORLD);

   }   

    if(myid!= 0) 
    {
    //make array for process column. Reuse D.
           for(j=0; j<dimension;j++) //make p0s column
           {
                D[j] = C[myid][j];
                //printf("%d ", D[j]);
                //now send completed column back to p0
            }
            MPI_Send(&D, dimension, MPI_INT, 0, myid, MPI_COMM_WORLD); 
    }

    MPI_Barrier(MPI_COMM_WORLD);
    //make sure all threads have sent to p0
    if(myid == 0) 
    {
          // for(j=0; j<dimension; j++) {
          //       printf("C[%d][%d] is %d\n",myid,j,C[myid][j] );
          //   }
    //receive column from other threads and add to C 
    for(i = 1; i < dimension; i++) 
    {
            MPI_Recv(&D, dimension, MPI_INT, i, i, MPI_COMM_WORLD, &status); 
        for(j=0; j<dimension; j++) {
            C[i][j] = D[j];
            //printf("%d ", D[j]);
        }
    }  

    //print C
            printf("\nA x B =\n");
    for (i = 0; i < dimension; i++) {
        for (j = 0; j < dimension; j++) {
            printf("%d ", C[i][j]);
            }
         printf("\n");
        }
        //printf("\n elapsed time: %d", elapsed_time);
    elapsed_time += MPI_Wtime();
    printf("time taken = %f.\n",elapsed_time);
    }

  MPI_Finalize();

    return 0;
}