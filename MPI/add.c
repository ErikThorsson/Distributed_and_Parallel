  #include <stdio.h>
  #include "mpi.h"
  #define MAXSIZE 35


  int main(argc,argv)
  int argc;
  char *argv[];
  {
      int pSize, myid, numprocs, x, lastX;
      int data[MAXSIZE];
      int i, j, low, high, myres = 0, res = 0, dest;

      MPI_Status status;
      MPI_Init(&argc,&argv);
      MPI_Comm_size(MPI_COMM_WORLD,&numprocs); //# processes
      MPI_Comm_rank(MPI_COMM_WORLD,&myid); //id

      x = MAXSIZE/(numprocs - 1); //partition size
      lastX = MAXSIZE - (numprocs - 2) * x; //which = maxsize - the last process (-2 bc there is master) * chunksize
     	int part[x];
      int lastPart[lastX];
      int lastIndex = (numprocs - 2) * x;

               if (myid == 0) //if first process, make array and send its pieces to each process
            {
                  for(i=0; i<MAXSIZE; i++) //setup initial full array based on MAXSIZE
                    {
                    	if(i < MAXSIZE/3)
                    		data[i]=1;
                    	else if(i < MAXSIZE/3 * 2)
                    		data[i]=2;
                    	else
                    		data[i]=3;

                    }

                    for(i = 0; i<numprocs - 1; i++)

                    {

                    	dest = i + 1; 

                      if(i < numprocs - 2) //if any partition but the last...send a normal partition array size
                      	{
                    	//printf("sending to part %d data index %d to %d\n", dest, i*x, i*x + x-1);
                        for(j=0;j<x;j++)
                      {
                        part[j] = data[i * x + j]; 
  					           }
  					           MPI_Send(&part, x, MPI_INT, dest, dest, MPI_COMM_WORLD);
  					           } else { //else send the last partition array size
  					             //printf("last partition\n");
                			   for(j=0;j<lastX;j++)
                				 {
                				//printf("index %d is %d\n", lastIndex + j, data[lastIndex + j]);
                  			lastPart[j] = data[lastIndex+ j]; //last partition begins at
             					  }
             		   	     MPI_Send(&lastPart, lastX, MPI_INT, dest, dest, MPI_COMM_WORLD);
             				   }
             			}
             		
            	} 
            	else if(myid < numprocs - 1) //else if not p0 and not last, receieve the array chunks and add
            	{

                //printf("received at process %d\n", myid);
            	  MPI_Recv(&part, x, MPI_INT, 0, myid, MPI_COMM_WORLD, &status);
            	      for(i=0;i<x;i++)
                {
  				      printf("%d ", part[i]);
                	myres+= part[i];
                }
                  printf("partition %d local sum is %d \n", myid, myres);
                }

               else //for last chunk size

               {
               //printf("received at process %d\n", myid);
               MPI_Recv(&lastPart, lastX, MPI_INT, 0, myid, MPI_COMM_WORLD, &status);
               for(i=0;i<lastX;i++)
                {
                	printf("%d ", lastPart[i]);
                	myres+= lastPart[i];
                }
                 	printf("partition %d local sum is %d\n", myid, myres);

                }

            MPI_Reduce(&myres, &res, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); //add all local sums

            if(myid ==0)
            {
            	printf("\nThe final sum is %d\n", res);
            }
            MPI_Finalize();
      		return 0;	
          }