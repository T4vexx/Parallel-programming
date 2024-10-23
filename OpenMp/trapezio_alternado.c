#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// função a ser integrada
double f(double x)
{
  return powf(x, 2) * (4 * powf(x, 3));
}

// Implementação da função trapezio que calcula a área
double trapezio(double x0, double x1)
{
  double y0 = f(x0);
  double y1 = f(x1);
  return ((y0 + y1) / 2.0) * (x1 - x0);
}

int main(int argc, char **argv)
{
  const double increment = 1e-6;
  double total_sum = 0;
  double partial_sum = 0;

  const double end = 1;
  const double begin = 0;
  const long divisions = (end - begin) / increment;

#pragma omp parallel private(partial_sum)
  {
    partial_sum = 0;
#pragma omp for
    for (long i = 0; i < divisions; i++)
    {
      double x0 = i * increment;
      double x1 = x0 + increment;
      partial_sum += trapezio(x0, x1);
    }
#pragma omp critical
    {
      total_sum += partial_sum;
    }
  }

  printf("Total: %f\n", total_sum);
  return 0;
}