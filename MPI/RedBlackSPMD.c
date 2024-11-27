#include <mpi.h>
#include <stdio.h>
#include <stdbool.h>

#define MAXTIME 100000
#define MINPROC 4
#define COLS 200
#define TOTROWS 200
#define ROWS TOTROWS / MINPROC + 2
double black[ROWS + 2][COLS + 2];
double red[ROWS + 2][COLS + 2];

void storeconst(int s, int e, int row, int col, double val)
{
  if (row >= s && row <= e)
  {
    black[row - s][col] = val;
    red[row - s][col] = val;
  }
}

void storeconsts(int s, int e)
{
  storeconst(s, e, TOTROWS / 3, COLS / 3, 10.0);
  storeconst(s, e, 2 * TOTROWS / 3, COLS / 3, 20.0);
  storeconst(s, e, TOTROWS / 3, 2 * COLS / 3, -20.0);
  storeconst(s, e, 2 * TOTROWS / 3, 2 * COLS / 3, 20.0);
}

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  int psize;
  MPI_Comm_size(MPI_COMM_WORLD, &psize);
  int dims = psize;

  MPI_Comm new_comm;
  int dimensions = 1;
  int periods = false;
  int reorder = true;
  MPI_Cart_create(MPI_COMM_WORLD, dimensions, &dims, &periods, reorder, &new_comm);

  int myrank;
  MPI_Comm_rank(new_comm, &myrank);

  int coords;
  MPI_Cart_get(new_comm, dimensions, &dims, &periods, &coords);

  int upproc, downproc;
  MPI_Cart_shift(new_comm, 0, 1, &upproc, &downproc);

  int section_start = TOTROWS / psize * myrank;
  int section_end = TOTROWS / psize * (myrank + 1);
  int mylen = (section_end - section_start) + 1;

  for (int r = 0; r <= mylen + 1; r++)
    for (int c = 0; c <= COLS + 1; c++)
      black[r][c] = 0.0;

  for (int tick = 1; tick <= MAXTIME; tick++)
  {
    storeconsts(section_start, section_end);
    MPI_Send(&black[1][0], COLS, MPI_DOUBLE, upproc, 1, new_comm);
    MPI_Recv(&black[mylen + 1][0], COLS, MPI_DOUBLE, downproc, 1, new_comm, MPI_STATUS_IGNORE);

    MPI_Sendrecv(&black[mylen][0], COLS, MPI_DOUBLE, downproc, 2, &black[0][0], COLS, MPI_DOUBLE, upproc, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int r = 1; r <= mylen; r++)
      for (int c = 1; c <= COLS; c++)
        red[r][c] = (black[r][c] + black[r][c - 1] + black[r - 1][c] + black[r + 1][c] + black[r][c + 1]) / 5.0;
    storeconsts(section_start, section_end);
    for (int r = 1; r <= mylen; r++)
      for (int c = 1; c <= COLS; c++)
        black[r][c] = (red[r][c] + red[r][c - 1] + red[r - 1][c] + red[r + 1][c] + red[r][c + 1]) / 5.0;
  }

  storeconsts(section_start, section_end);
  if (myrank == 1)
  {
    for (int r = 12; r <= 22; r++)
    {
      for (int c = 62; c <= 70; c++)
        printf("%lf ", black[r][c]);
      puts(" ");
    }
  }
  MPI_Finalize();
}