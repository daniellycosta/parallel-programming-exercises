/* File:    q13.c
 *
 * Purpose:  A program in which multiple MPI processes receive a slice of two vectors (v1,v2)
 * and a escalar (a) and makes: (a*v1) * v2
 * 
 * Compile:  mpicc -g -Wall -o q13 q13.c
 * Usage:    mpiexec -n <number of processes> ./q13
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
void Get_displ_send_count(int *displs, int *send_count, int n, int my_rank, int comm_sz, MPI_Comm comm);
void Read_vector(char vector_name[], int local_vec[], int *displs, int *send_count, int n, int local_n, int my_rank, MPI_Comm comm);
void Esc_vect_mult(int local_V1[], int local_V2[], int a, int local_y[], int n, int local_n, MPI_Comm comm);
void Print_answer(int local_vec[], int n, int local_n, int my_rank, int *displs, int *send_count, MPI_Comm comm);

int main(void)
{
  int my_rank, comm_sz;
  int local_n, a, n;
  int *local_V1;
  int *local_V2;
  int *local_y;
  int *displs = NULL;
  int *send_count = NULL;

  MPI_Comm comm = MPI_COMM_WORLD;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  Read_Escalars(&a, &n, &local_n, my_rank, comm_sz, comm);

  local_V1 = malloc(local_n * sizeof(int));
  local_V2 = malloc(local_n * sizeof(int));
  local_y = malloc(local_n * sizeof(int));
  displs = malloc(n * sizeof(int));
  send_count = malloc(n * sizeof(int));

  Get_displ_send_count(displs, send_count, n, my_rank, comm_sz, comm);

#ifdef DEBUG
  if (!my_rank)
  {
    printf("[DEBUG]: \n a:%d, n:%d, local_n:%d\n", a, n, local_n);
    printf("Sendcount:\n");
    for (int i = 0; i < comm_sz; i++)
      printf(" %d", send_count[i]);
    printf("\ndispls:\n");
    for (int i = 0; i < comm_sz; i++)
      printf(" %d", displs[i]);
    printf("\n");
  }
#endif

  Read_vector("V1", local_V1, displs, send_count, n, local_n, my_rank, comm);
  Read_vector("V2", local_V2, displs, send_count, n, local_n, my_rank, comm);

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

  Print_answer(local_y, n, local_n, my_rank, displs, send_count, comm);

  free(local_V1);
  free(local_V2);
  free(local_y);

  MPI_Finalize();
  return 0;
} /* main */

void Read_vector(
    char vector_name[] /*in*/,
    int local_vec[] /*out*/,
    int *displs /*in*/,
    int *send_count /*in*/,
    int n /*in*/,
    int local_n /*in*/,
    int my_rank /*in*/,
    MPI_Comm comm /*in*/)
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

    MPI_Scatterv(vec, send_count, displs, MPI_INT, local_vec, local_n, MPI_INT, 0, comm);
    free(vec);
  }
  else
  {
    MPI_Scatterv(vec, send_count, displs, MPI_INT, local_vec, local_n, MPI_INT, 0, comm);
  }

} /* Read_vector */
void Get_displ_send_count(
    int *displs,
    int *send_count,
    int n,
    int my_rank,
    int comm_sz,
    MPI_Comm comm)
{

  int elems_per_process = n / comm_sz;
  int remaining_elems = n % comm_sz;
  int local_n;
  if (!my_rank)
  {
    for (int i = 0; i < comm_sz; i++)
    {
      if (i < remaining_elems)
      {
        local_n = elems_per_process + 1;
        displs[i] = i * local_n;
      }
      else
      {
        local_n = elems_per_process;
        displs[i] = i * local_n + remaining_elems;
      }
      send_count[i] = local_n;
    }
  }

  MPI_Bcast(displs, comm_sz, MPI_INT, 0, comm);
  MPI_Bcast(send_count, comm_sz, MPI_INT, 0, comm);

} /*Get_displ_send_count*/

void Read_Escalars(
    int *a /* out */,
    int *n_p /* out */,
    int *local_n_p /* out */,
    int my_rank /* in  */,
    int comm_sz /* in  */,
    MPI_Comm comm /* in  */)
{

  if (!my_rank)
  {
    printf("Enter the number of elements\n");
    scanf("%d", n_p);
    printf("Enter the escalar\n");
    scanf("%d", a);
  }
  MPI_Bcast(a, 1, MPI_INT, 0, comm);
  MPI_Bcast(n_p, 1, MPI_INT, 0, comm);

  int elems_per_process = *n_p / comm_sz;
  int remaining_elems = *n_p % comm_sz;

  if (my_rank < remaining_elems)
  {
    *local_n_p = elems_per_process + 1;
  }
  else
  {
    *local_n_p = elems_per_process;
  };

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
    int *displs /*out*/,
    int *send_count /*out*/,
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
    /*
    int MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
    void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype
recvtype,
    int root, MPI_Comm comm)

    */
    MPI_Gatherv(local_vec, local_n, MPI_INT, answ, send_count,
                displs, MPI_INT, 0, comm);

    printf("\nThe Result: \n");
    for (i = 0; i < n; i++)
      printf(" %d", answ[i]);
    printf("\n");
    free(answ);
  }
  else
  {
    MPI_Gatherv(local_vec, local_n, MPI_INT, answ, send_count,
                displs, MPI_INT, 0, comm);
  }
} /* Print_answer */
