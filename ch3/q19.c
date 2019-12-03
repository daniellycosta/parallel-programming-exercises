/* File:    q19.c
 *
 * Purpose:  A program in which multiple MPI processes receive a slice of two vectors (v1,v2)
 * and a escalar (a) and makes: (a*v1) * v2
 * 
 * Compile:  mpicc -g -Wall -o q19 q09.c
 * Usage:    mpiexec -n <number of processes> ./q19
 *
 * Input:    arrays size, escalar, array elements
 * Output:   Answer vector
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
//#define DEBUG true

void Read_Escalars(int *n_p, int my_rank, MPI_Comm comm);
void Read_Matrix(char vector_name[], int matrix[], int n, int my_rank, MPI_Datatype type, MPI_Comm comm);
void Print_answer(int matrix[], int n);

int main(void)
{
  int my_rank, comm_sz;
  int n;
  int *matrix, *block_len, *displ;

  MPI_Comm comm = MPI_COMM_WORLD;
  MPI_Datatype my_datatype;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  Read_Escalars(&n, my_rank, comm);

  block_len = malloc(n * sizeof(int));
  displ = malloc(n * sizeof(int));

  for (int i = 0; i < n; i++)
  {
    block_len[i] = n - i;
    displ[i] = i * (n + 1);
  }

  MPI_Type_indexed(n, block_len, displ, MPI_INT, &my_datatype);
  MPI_Type_commit(&my_datatype);

  matrix = malloc(n * n * sizeof(int));

  Read_Matrix("Matrix", matrix, n, my_rank, my_datatype, comm);

  if (my_rank)
    Print_answer(matrix, n);

  free(matrix);
  MPI_Finalize();
  return 0;
} /* main */

void Read_Matrix(
    char vector_name[] /* in  */,
    int matrix[] /* out */,
    int n /* in  */,
    int my_rank /* in  */,
    MPI_Datatype type,
    MPI_Comm comm /* in  */)
{
  int i;

  if (my_rank == 0)
  {
    printf("Enter the matrix %s (%d elements)\n", vector_name, n * n);
    for (i = 0; i < n * n; i++)
      scanf("%d", &matrix[i]);

    MPI_Send(matrix, 1, type, 1, 0, comm);
  }
  {
    MPI_Recv(matrix, 1, type, 0, 0, comm, MPI_STATUS_IGNORE);
  }

} /* Read_vector */

void Read_Escalars(
    int *n_p /* out */,
    int my_rank /* in  */,
    MPI_Comm comm /* in  */)
{

  if (my_rank == 0)
  {
    printf("Enter the number of elements\n");
    scanf("%d", n_p);
  }
  MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
} /* Read_Escalars */

void Print_answer(
    int matrix[] /* in */,
    int n /* in */)
{

  int i, j;

  printf("\nThe Result: \n");
  for (i = 0; i < n; i++)
  {
    for (j = i; j < n; j++)
    {
      printf("%d ", matrix[i * n + j]);
    }
    printf("\n");
  }
  printf("\n");

} /* Print_answer */
