// Código que simula um algoritmo red and black funcionado sobre uma chapa de metal
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define cols 200
#define rows 200
#define max_time 180000

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "Usage: red_black <nthreads>\n Exiting...\n");
    exit(1);
  }

  double red[rows + 2][cols + 2], black[rows + 2][cols + 2];

  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      red[i][j] = 0;
      black[i][j] = 0;
    }
  }

  // Definição de quantas threads trabalharão em paralelo
  int thread_count = strtol(argv[1], NULL, 10);
  omp_set_num_threads(thread_count);

  for (int tick = 0; tick < max_time; tick++)
  {
    black[rows / 3][cols / 3] = 10.0;
    black[2 * rows / 3][cols / 3] = 20.0;
    black[rows / 3][2 * cols / 3] = -20.0;
    black[2 * rows / 3][2 * cols / 3] = 20.0;

#pragma omp parallel shared(black, red, thread_count)
    {
#pragma omp for schedule(dynamic, rows / thread_count)
      for (int r = 1; r <= rows; r++)
      {
        for (int c = 1; c <= cols; c++)
        {
          red[r][c] = (black[r][c] + black[r][c - 1] + black[r][c + 1] + black[r - 1][c] + black[r + 1][c]) / 5.0;
        }
      }
    }

    red[rows / 3][cols / 3] = 10.0;
    red[2 * rows / 3][cols / 3] = 20.0;
    red[rows / 3][2 * cols / 3] = -20.0;
    red[2 * rows / 3][2 * cols / 3] = 20.0;

#pragma omp parallel shared(black, red, thread_count)
    {
#pragma omp for schedule(dynamic, rows / thread_count)
      for (int r = 1; r <= rows; r++)
      {
        for (int c = 1; c <= cols; c++)
        {
          black[r][c] = (black[r][c] + black[r][c - 1] + black[r][c + 1] + black[r - 1][c] + black[r + 1][c]) / 5.0;
        }
      }
    }
  }

  // reatribui os valores constantes
  black[rows / 3][cols / 3] = 10.0;
  black[2 * rows / 3][cols / 3] = 20.0;
  black[rows / 3][2 * cols / 3] = -20.0;
  black[2 * rows / 3][2 * cols / 3] = 20.0;

  for (int i = 62; i < 72; i++)
  {
    for (int j = 62; j < 70; j++)
    {
      printf("%lf ", black[i][j]);
    }
    puts(" ");
  }

  return 0;
}