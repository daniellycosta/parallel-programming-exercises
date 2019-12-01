/* File:    q12_b.c
 *
 * Purpose:  A program in which multiple MPI processes receive a slice of two vectors (v1,v2)
 * and a escalar (a) and makes: (a*v1) * v2
 * 
 * Compile:  mpicc -g -Wall -o q12_b q12_b.c
 * Usage:     mpiexec -n <number of processes> ./q12_b
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
void Print_answer(int prefix_sum[], int *local_el, int my_rank, int comm_sz, MPI_Comm comm);

int main(void)
{
  int my_rank, comm_sz;
  int my_val = 0;
  int temp_val = 0;
  int dest, source;
  int *sum = NULL;
  MPI_Comm comm = MPI_COMM_WORLD;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  sum = malloc(comm_sz * sizeof(int));

  Read_vector("el", &my_val, comm_sz, my_rank, comm);

#ifdef DEBUG
  printf("[DEBUG Rank: %d] El: %d \n", my_rank, my_val);
#endif

  if (!my_rank)
  {
    dest = my_rank + 1;

    MPI_Send(&my_val, 1, MPI_INT, dest, 0, comm);
  }
  else if (my_rank != (comm_sz - 1))
  {

    dest = my_rank + 1;
    source = my_rank - 1;

    MPI_Recv(&temp_val, 1, MPI_INT, source, 0, comm, MPI_STATUS_IGNORE);
    my_val = temp_val + my_val;
    MPI_Send(&my_val, 1, MPI_INT, dest, 0, comm);
  }
  else
  {
    source = my_rank - 1;

    MPI_Recv(&temp_val, 1, MPI_INT, source, 0, comm, MPI_STATUS_IGNORE);
    my_val = temp_val + my_val;
  }

  Print_answer(sum, &my_val, my_rank, comm_sz, comm);
  free(sum);
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

void Print_answer(int prefix_sum[], int *local_el, int my_rank, int comm_sz, MPI_Comm comm)
{
  MPI_Gather(local_el, 1, MPI_INT, prefix_sum, 1, MPI_INT, 0, comm);
  if (!my_rank)
  {
    printf("Answer:\n");
    for (int i = 0; i < comm_sz; i++)
      printf(" %d,", prefix_sum[i]);
    printf("\n");
  }

} /*Print Answer*/
