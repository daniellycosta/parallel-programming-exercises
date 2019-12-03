/* File:    q09.c
 *
 * Purpose:  A program in which multiple MPI processes receive a slice of two vectors (v1,v2)
 * and a escalar (a) and makes: (a*v1) * v2
 * 
 * Compile:  mpicc -g -Wall -o q09 q09.c
 * Usage:    mpiexec -n <number of processes> ./q09
 *
 * Input:    arrays size, escalar, array elements
 * Output:   Answer vector
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
//#define DEBUG true

void Read_Escalars(int *a, int *n_p, int *local_n_p, int my_rank, int comm_sz, MPI_Comm comm);
void Read_vector(char vector_name[], int local_vec[], int n, int local_n, int my_rank, MPI_Datatype my_datatype, MPI_Comm comm);
void Esc_vect_mult(int local_V1[], int local_V2[], int a, int local_y[], int n, int local_n, MPI_Comm comm);
void Print_answer(int local_vec[], int n, int local_n, int my_rank, MPI_Datatype my_datatype, MPI_Comm comm);

int main(void)
{
  int my_rank, comm_sz;
  int local_n, a, n;
  int *local_V1;
  int *local_V2;
  int *local_y;

  MPI_Comm comm = MPI_COMM_WORLD;
  MPI_Datatype my_datatype;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  Read_Escalars(&a, &n, &local_n, my_rank, comm_sz, comm);

  MPI_Type_contiguous(local_n, MPI_INT, &my_datatype);
  MPI_Type_commit(&my_datatype);

#ifdef DEBUG
  if (!my_rank)
    printf("[DEBUG]: \n a:%d, n:%d, local_n:%d\n", a, n, local_n);
#endif

  local_V1 = malloc(local_n * sizeof(int));
  local_V2 = malloc(local_n * sizeof(int));
  local_y = malloc(local_n * sizeof(int));

  Read_vector("V1", local_V1, n, local_n, my_rank, my_datatype, comm);
  Read_vector("V2", local_V2, n, local_n, my_rank, my_datatype, comm);

#ifdef DEBUG
  printf("[DEBUG] Rank %d: \n V1:", my_rank);
  for (int i = 0; i < local_n; i++)
    printf(" %d", local_V1[i]);
  printf("\n V2:");
  for (int i = 0; i < local_n; i++)
    printf(" %d", local_V2[i]);
  printf("\n");
#endif

  Esc_vect_mult(local_V1, local_V2, a, local_y, n, local_n, comm);

#ifdef DEBUG
  printf("[DEBUG] Rank %d: \n Y:", my_rank);
  for (int i = 0; i < local_n; i++)
    printf(" %d", local_y[i]);
  printf("\n");
#endif

  Print_answer(local_y, n, local_n, my_rank, my_datatype, comm);

  free(local_V1);
  free(local_V2);
  free(local_y);

  MPI_Finalize();
  return 0;
} /* main */

void Read_vector(
    char vector_name[] /* in  */,
    int local_vec[] /* out */,
    int n /* in  */,
    int local_n /* in  */,
    int my_rank /* in  */,
    MPI_Datatype type,
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

    MPI_Scatter(vec, local_n, type, local_vec, local_n, type, 0, comm);
    free(vec);
  }
  else
  {
    MPI_Scatter(vec, local_n, type,
                local_vec, local_n, type, 0, comm);
  }
} /* Read_vector */

void Read_Escalars(
    int *a /* out */,
    int *n_p /* out */,
    int *local_n_p /* out */,
    int my_rank /* in  */,
    int comm_sz /* in  */,
    MPI_Comm comm /* in  */)
{

  if (my_rank == 0)
  {
    printf("Enter the number of elements\n");
    scanf("%d", n_p);
    printf("Enter the escalar\n");
    scanf("%d", a);
  }
  MPI_Bcast(a, 1, MPI_INT, 0, comm);
  MPI_Bcast(n_p, 1, MPI_INT, 0, comm);

  *local_n_p = *n_p / comm_sz;
} /* Read_Escalars */

void Esc_vect_mult(
    int local_V1[] /* in  */,
    int local_V2[] /* in  */,
    int a /* in  */,
    int local_y[] /* out */,
    int n /* in  */,
    int local_n /* in  */,
    MPI_Comm comm /* in  */)
{
  int local_i;
  for (local_i = 0; local_i < local_n; local_i++)
  {
    local_y[local_i] = a * local_V1[local_i];
    local_y[local_i] = local_y[local_i] * local_V2[local_i];
  }

} /* Esc_vect_mult */

void Print_answer(
    int local_vec[] /* in */,
    int n /* in */,
    int local_n /* in */,
    int my_rank /* in */,
    MPI_Datatype type,
    MPI_Comm comm /* in */)
{
  int *answ = NULL;
  int i;

#ifdef DEBUG
  printf("[DEBUG - Print Answer] Rank %d : \n", my_rank);
  for (int i = 0; i < local_n; i++)
    printf(" %d", local_vec[i]);
  printf("\n");
#endif

  if (my_rank == 0)
  {
    answ = malloc(n * sizeof(int));
    MPI_Gather(local_vec, local_n, type,
               answ, local_n, type, 0, comm);

    printf("\nThe Result: \n");
    for (i = 0; i < n; i++)
      printf(" %d", answ[i]);
    printf("\n");
    free(answ);
  }
  else
  {
    MPI_Gather(local_vec, local_n, type,
               answ, local_n, type, 0, comm);
  }
} /* Print_answer */
