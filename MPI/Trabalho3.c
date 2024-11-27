#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define A_X 0.0
#define B_X 1.5
#define A_Y 0.0
#define B_Y 1.5

double f(double x, double y)
{
  return sin(x * x + y * y);
}

int main(int argc, char *argv[])
{
  int rank, size;
  double local_integral = 0.0, global_integral = 0.0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc != 3)
  {
    if (rank == 0)
    {
      fprintf(stderr, "Uso: mpirun -np <num_procs> %s <num_intervals_x> <num_intervals_y>\n", argv[0]);
    }
    MPI_Finalize();
    return 1;
  }

  int num_intervalos_x = atoi(argv[1]);
  int num_intervalos_y = atoi(argv[2]);

  const double delta_x = (B_X - A_X) / num_intervalos_x;
  const double delta_y = (B_Y - A_Y) / num_intervalos_y;

  // Dividir o trabalho entre os processos
  int rows_per_proc = num_intervalos_x / size;
  int start_row = rank * rows_per_proc;
  int end_row = (rank == size - 1) ? num_intervalos_x : start_row + rows_per_proc;

  // Cálculo da integral local
  for (int i = start_row; i <= end_row; i++)
  {
    for (int j = 0; j <= num_intervalos_y; j++)
    {
      double x_i = A_X + i * delta_x;
      double y_j = A_Y + j * delta_y;

      double weight = 1.0;
      if ((i == 0 || i == num_intervalos_x) && (j == 0 || j == num_intervalos_y))
      {
        weight = 0.25;
      }
      else if (i == 0 || i == num_intervalos_x || j == 0 || j == num_intervalos_y)
      {
        weight = 0.5;
      }

      local_integral += weight * f(x_i, y_j);
    }
  }

  local_integral *= delta_x * delta_y;

  // Redução para somar todas as integrais locais
  MPI_Reduce(&local_integral, &global_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0)
  {
    printf("O valor aproximado da integral dupla e: %lf\n", global_integral);
  }

  MPI_Finalize();
  return 0;
}