/* File:    q9.c
 *
 * Purpose:  A program in which multiple MPI processes receive a slice of two vectors (v1,v2)
 * and a escalar (a) and makes: (a*v1) * v2
 * 
 * Compile:  mpicc -g -Wall -o q11_b q11_b.c
 * Usage:    mpiexec -n <number of processes> ./q11_b
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
void Send_recv_sum(int *received_value, int *local_el, int my_rank, int rank_dest, int rank_src, int comm_sz, MPI_Comm comm);
void Print_answer(int *local_el, int my_rank, int comm_sz, MPI_Comm comm);

int main(void)
{
  int my_rank, comm_sz;
  int local_X = 0;
  int received_value = 0;
  int rank_send, rank_recv;

  MPI_Comm comm = MPI_COMM_WORLD;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  Read_vector("X", &local_X, comm_sz, my_rank, comm);
#ifdef DEBUG
  printf("[DEBUG]: \n my_rank:%d, comm_sz:%d, local_x:%d\n", my_rank, comm_sz, local_X);
#endif

  for (int i = 1; i < comm_sz; i *= 2)
  {
    rank_send = my_rank + i;
    rank_recv = my_rank - i;
    Send_recv_sum(&received_value, &local_X, my_rank, rank_send, rank_recv, comm_sz, comm);
  }

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
    int *received_value /*in*/,
    int *local_el /* in */,
    int my_rank /* in */,
    int rank_dest /* in */,
    int rank_src /* in */,
    int comm_sz /* in */,
    MPI_Comm comm /* in */)
{

  if (rank_dest < comm_sz - 1)
  {
    int local_sum = *received_value + *local_el;
    printf("local_sum = %d\n", local_sum);

#ifdef DEBUG
    printf("[DEBUG]- valor enviado de %d para %d: \n", my_rank, rank_dest);
    printf("%d", local_sum);
    printf("\n");
#endif
    MPI_Send(&local_sum, sizeof(int), MPI_INT, rank_dest, 0, comm);
  }
  if (rank_src >= 0)
    MPI_Recv(&received_value, sizeof(int), MPI_INT, rank_src, 0, comm, MPI_STATUS_IGNORE);

#ifdef DEBUG
  printf("[DEBUG]- valor recebido em  %d: \n", my_rank);
  printf("%d", *received_value);
  printf("\n");
#endif

} /*Send_recv_sum*/

void Print_answer(
    int *local_el /* in */,
    int my_rank /* in */,
    int comm_sz /* in */,
    MPI_Comm comm /* in */)
{

#ifdef DEBUG
  printf("[DEBUG - Print Answer Rank %d : \n", my_rank);
  printf(" %d", *local_el);
  printf("\n");
#endif
  int *answ = NULL;
  int i;

  if (my_rank == 0)
  {
    answ = malloc(comm_sz * sizeof(int));
    MPI_Gather(&local_el, 1, MPI_INT,
               answ, 1, MPI_INT, 0, comm);

    printf("\nThe Result: \n");
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
