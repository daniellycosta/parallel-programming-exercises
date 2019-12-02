/* File:    q11_d.c
 *
 * Purpose:  A program in which multiple MPI processes receive a slice of two vectors (v1,v2)
 * and a escalar (a) and makes: (a*v1) * v2
 * 
 * Compile:  mpicc -g -Wall -o q11_d q11_d.c
 * Usage:    mpiexec -n <number of processes> ./q11_d
 *
 * Input:    arrays size, escalar, array elements
 * Output:   Answer vector
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
//#define DEBUG true

void Read_vector(char vector_name[], int *local_el, int n, int my_rank, MPI_Comm comm);
void Send_recv_sum(int *local_el, int my_rank, int comm_sz, MPI_Comm comm);
void Print_answer(int *local_el, int my_rank, int comm_sz, MPI_Comm comm);

int main(void)
{
  int my_rank, comm_sz;
  int local_X = 0;

  MPI_Comm comm = MPI_COMM_WORLD;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  Read_vector("X", &local_X, comm_sz, my_rank, comm);
#ifdef DEBUG
  printf("[DEBUG]: my_rank:%d,local_x:%d\n", my_rank, local_X);
#endif

  Send_recv_sum(&local_X, my_rank, comm_sz, comm);

  Print_answer(&local_X, my_rank, comm_sz, comm);

  MPI_Finalize();
  return 0;
} /* main */

void Read_vector(
    char vector_name[] /* in  */,
    int *local_el /* out */,
    int n /* in  */,
    int my_rank /* in  */,
    MPI_Comm comm /* in  */)
{
  int *vec = NULL;
  int i;

  if (my_rank == 0)
  {
    vec = malloc(n * sizeof(int));
    printf("Enter the vector %s\n", vector_name);
    for (i = 0; i < n; i++)
      scanf("%d", &vec[i]);

#ifdef DEBUG
    printf("[DEBUG]- Vector %s: \n", vector_name);
    for (int i = 0; i < n; i++)
      printf(" %d", vec[i]);
    printf("\n");
#endif

    MPI_Scatter(vec, 1, MPI_INT, local_el, 1, MPI_INT, 0, comm);
    free(vec);
  }
  else
  {
    MPI_Scatter(vec, 1, MPI_INT, local_el, 1, MPI_INT, 0, comm);
  }
} /* Read_vector */

void Send_recv_sum(
    int *local_el /* in */,
    int my_rank /* in */,
    int comm_sz /* in */,
    MPI_Comm comm /* in */)
{
  int local_sum = 0;

  MPI_Scan(local_el, &local_sum, 1, MPI_INT, MPI_SUM, comm);
  *local_el = local_sum;

} /*Send_recv_sum*/

void Print_answer(
    int *local_el /* in */,
    int my_rank /* in */,
    int comm_sz /* in */,
    MPI_Comm comm /* in */)
{

#ifdef DEBUG
  printf("[DEBUG - Print Answer Rank %d: %d \n", my_rank, *local_el);
#endif
  int *answ = NULL;
  int i;

  if (my_rank == 0)
  {
    answ = malloc(comm_sz * sizeof(int));
    MPI_Gather(local_el, 1, MPI_INT,
               answ, 1, MPI_INT, 0, comm);

    printf("\n\nThe Result: \n");
    for (i = 0; i < comm_sz; i++)
      printf(" %d", answ[i]);
    printf("\n");
    free(answ);
  }
  else
  {
    MPI_Gather(local_el, 1, MPI_INT,
               answ, 1, MPI_INT, 0, comm);
  }
} /* Print_answer */
