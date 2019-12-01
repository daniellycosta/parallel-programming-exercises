/* File:    q12_a.c
 *
 * Purpose:  A program in which multiple MPI processes receive a slice of two vectors (v1,v2)
 * and a escalar (a) and makes: (a*v1) * v2
 * 
 * Compile:  mpicc -g -Wall -o q12 q12.c
 * Usage:     mpiexec -n <number of processes> ./q12_a
 *
 * Input:    arrays size, escalar, array elements
 * Output:   Answer vector
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#define DEBUG true

void Read_vector(char vector_name[], int *local_el, int comm_sz, int my_rank, MPI_Comm comm);
void Print_answer(int *local_el, int my_rank, MPI_Comm comm);

int main(void)
{
  int my_rank, comm_sz;
  int my_val = 0;
  int temp_val = 0;
  int sum = 0;
  int i, dest, source;
  MPI_Comm comm = MPI_COMM_WORLD;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  Read_vector("el", &my_val, comm_sz, my_rank, comm);
#ifdef DEBUG
  printf("[DEBUG Rank: %d] El: %d \n", my_rank, my_val);
#endif

  sum = 0;
  if (!my_rank)
  {
    dest = my_rank + 1;
    source = comm_sz - 1;
  }
  else if (my_rank != comm_sz - 1)
  {

    dest = my_rank + 1;
    source = my_rank - 1;
  }
  else
  {
    dest = 0;
    source = my_rank - 1;
  }
  sum = temp_val = my_val;

#ifdef DEBUG
  printf("[DEBUG Rank: %d] dest: %d, src:%d \n", my_rank, dest, source);
#endif

  for (i = 1; i < comm_sz; i++)
  {

    MPI_Sendrecv_replace(&temp_val, 1, MPI_INT, dest, 0, source, 0, comm, MPI_STATUS_IGNORE);
    sum += temp_val;
  }
  Print_answer(&sum, my_rank, comm);

  MPI_Finalize();
  return 0;
} /* main */

void Read_vector(
    char vector_name[] /* in  */,
    int *local_el /* out */,
    int comm_sz /* in  */,
    int my_rank /* in  */,
    MPI_Comm comm /* in  */)
{
  int *vec = NULL;
  int i;
  if (my_rank == 0)
  {
    vec = malloc(comm_sz * sizeof(int));
    printf("Enter the vector %s\n", vector_name);
    for (i = 0; i < comm_sz; i++)
      scanf("%d", &vec[i]);

#ifdef DEBUG
    printf("[DEBUG]- Vector %s: \n", vector_name);
    for (int i = 0; i < comm_sz; i++)
      printf(" %d", vec[i]);
    printf("\n");
#endif
    MPI_Scatter(vec, 1, MPI_INT, local_el, 1, MPI_INT, 0, comm);
  }
  else
  {
    MPI_Scatter(vec, 1, MPI_INT, local_el, 1, MPI_INT, 0, comm);
  }
  free(vec);
} /* Read_vector */

void Print_answer(int *local_el, int my_rank, MPI_Comm comm)
{

  printf("my_Rank: %d \n Answer: %d \n", my_rank, *local_el);

} /*Print Answer*/
