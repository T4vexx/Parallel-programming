#include <stdio.h>
#include <omp.h>
#define N 100

int main(int argc, char *argv[])
{
  int array[N];
  int total_sum = 0;

  for (int i = 0; i < N; i++)
  {
    array[i] = i + 1;
  }

  #pragma omp parallel
  {
    int part_sum = 0;
    #pragma omp for reduction(+ : total_sum)
      for (int i = 0; i < N; i++)
      {
        total_sum += array[i];
        part_sum += array[i];
      }
      printf("Part sum: %d, thread %d\n", part_sum, omp_get_thread_num());
  }

  printf("Total sum: %d\n", total_sum);
  return 0;
}