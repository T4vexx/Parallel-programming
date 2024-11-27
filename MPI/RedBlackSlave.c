#include <mpi.h>
#include <stdio.h>

#define MAXTIME 100000
#define MINPROC 4
#define COLS 200
#define ROWS 200

double black[ROWS + 2][COLS + 2];
double red[ROWS + 2][COLS + 2];

void setup_consts()
{
  black[ROWS / 3][COLS / 3] = 1.0;
  black[2 * ROWS / 3][COLS / 3] = 20.0;
  black[ROWS / 3][2 * COLS / 3] = -20.0;
  black[2 * ROWS / 3][2 * COLS / 3] = 20.0;
}

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  int nproc;
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  int myrank;
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  if (myrank == 0)
  {
    for (int r = 0; r <= ROWS + 1; r++)
    {
      for (int c = 0; c <= COLS + 1; c++)
      {
        black[r][c] = 0.0;
      }
    }
  }
  const int start = myrank * (ROWS / nproc) + 1;
  const int end = start + (ROWS / nproc) - 1;

  for (int tick = 1; tick <= MAXTIME; tick++)
  {
    setup_consts();
    MPI_Bcast(black, (ROWS + 2) * (COLS + 2), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int r = start; r <= end; r++)
      for (int c = 1; c <= COLS; c++)
        red[r][c] = (black[r][c] + black[r][c - 1] + black[r - 1][c] + black[r + 1][c] + black[r][c + 1]) / 5.0;
    if (myrank == 0)
    {
      for (int r = start; r <= end; r++)
        for (int c = 1; c <= ROWS; c++)
          black[r][c] = red[r][c];

      for (int i = 1; i <= nproc - 1; i++)
      {
        const int local_start = 1 + i * (ROWS / nproc);
        const int local_end = local_start + (ROWS / nproc) - 1;
        const int mylen = (local_end - local_start) + 1;
        const int src = i;
        const int tag = 0;
        MPI_Recv(&black[local_start][0], mylen * (COLS + 2), MPI_DOUBLE, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
    else
    {
      const int mylen = (end - start) + 1;
      const int dest = 0;
      const int tag = 0;
      MPI_Send(&red[start][0], mylen * (COLS + 2), MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
    }
  }

  if (myrank == 0)
  {
    for (int r = 62; r <= 72; r++)
    {
      for (int c = 62; c <= 70; c++)
        printf("%lf ", black[r][c]);
      puts(" ");
    }
  }
  MPI_Finalize();
}
