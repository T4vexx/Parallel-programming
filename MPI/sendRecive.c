#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  int rank, size, number;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0)
  {
    number = 100;
    MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    printf("Process 0 sent number %d to Process 1\n", number);
  }
  else if (rank == 1)
  {
    MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process 1 recive number %d from Process 0\n", number);
  }

  MPI_Finalize();
  return 0;
}
