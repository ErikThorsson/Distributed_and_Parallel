/*
Erik Orndahl 9/25/15
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct
{
  int *p;
  int size;
}chunk;


struct thread_data{
int row; // row = number of rows in the matrix int 
int col; // col = number of columns in the matrix 
int p; // number of processors
int tid;
}thread_data;

struct thread_data thread_data_array[4];

chunk breakIntoChunks(int *A, int n, int p, int id) {
 
 	int i;
	int partitionSize = n/p+1; // a 4x4 would give a pSize of 6
  //which means the largest amount of work would be 6 operations by one
  //processor
	int *partition = (int *)malloc(partitionSize * sizeof(int));
  //each process will need a word's memory
   
	int counter = 0;
	
//assigns each cell to a processor based on original id and # processors
	for(i = id; i<n+1; i=i+p) {
    //printf("i is %d counter is %d\n", i, counter);
	  partition[counter] = i; 
	  counter++;
	}
	
  //object contains the # of processes and their locations
	chunk mychunk;
    mychunk.p = partition;
    mychunk.size = counter;
      
    return mychunk;
}

int display(chunk *myChunk) {
  
  int counter;
  int sum = 0;

  //prints chunk using object attributes
  for(counter = 0; counter<myChunk->size; counter++) {
   //printf("%d ",myChunk->p[counter]);
    sum+= myChunk->p[counter];
   }  
  return sum;
}


void *sum(void *threadarg)
{
   struct thread_data *my_data;
   my_data = (struct thread_data *) threadarg;
   int taskid, row, col, p;
   row = my_data->row;
   col = my_data->col;
   taskid = my_data->tid;
   p = my_data->p;

  int n = row * col;
  int x;
  int arr[n];
  
  for(int x= 0; x<n; x++) {
    arr[x] = x;
  } 

   chunk chunk0 = breakIntoChunks(arr, n, p, taskid);
   int sum;
   sum = display(&chunk0);
   printf("P%d: is %d\n",taskid - 1, sum);

   //printf("row:%d col:%d id:%d sum:%d\n", row, col, taskid, p);
   
}

int main(int argc, char *argv[]) {
  
 /*First identify the chunks of the matrix*/

  int row;
  int col;
  int p;
  int i = 0;
  
  sscanf (argv[1],"%d",&row);
  sscanf (argv[2],"%d",&col);
  sscanf (argv[3],"%d",&p);
  
   pthread_t threads[p];
   int rc;
   long t;
   for(t=1; t<=p; t++){
    
    thread_data_array[t].row= row;
    thread_data_array[t].col = col;
    thread_data_array[t].tid = t;
    thread_data_array[t].p = p;
  
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, sum, (void *) &thread_data_array[t]);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }

   /* Last thing that main() should do */
   pthread_exit(NULL);
  
  return 0;
}