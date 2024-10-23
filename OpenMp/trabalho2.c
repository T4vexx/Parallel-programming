// Otávio Augusto Teixeira
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define A_X 0.0 // Limite inferior de x
#define B_X 1.5 // Limite superior de x
#define A_Y 0.0 // Limite inferior de y
#define B_Y 1.5 // Limite superior de y

// Função f(x,y)
double f(double x, double y)
{
  return sin(x * x + y * y);
}

/*
  Levei em consideração a seguinte ideia matemática
  A aproximação da integral é (usando código LaTeX):
  I \approx \int_{i=0}^{N_x} \sum_{j=0}^{N_y} \sin(x_i^2 + y_j^2) \cdot w_{i,j} \cdot \Delta x \cdot \Delta y
*/

int main(int argc, char *argv[])
{
  // Recebo o número de threads para ser usado e o número de intervalos
  int num_threads, num_intervalos_x_y;
  if (argc < 3)
  {
    fprintf(stderr, "Uso correto: ./trabalho2 <num_threads> <num_intervals_x_y>\nExiting...\n");
    exit(1);
  }

  num_threads = strtol(argv[1], NULL, 10);
  num_intervalos_x_y = strtol(argv[2], NULL, 10);

  const double delta_x = (B_X - A_X) / num_intervalos_x_y;
  const double delta_y = (B_Y - A_Y) / num_intervalos_x_y;
  double integral = 0.0;

// Laço que utiliza o método do trapézio para fazer as aproximações
#pragma omp parallel for num_threads(num_threads) reduction(+ : integral)
  for (int i = 0; i <= num_intervalos_x_y; i++)
  {
    for (int j = 0; j <= num_intervalos_x_y; j++)
    {
      double x_i = A_X + i * delta_x;
      double y_j = A_Y + j * delta_y;

      // Calculo dos pesos de canto e borda
      double weight = 1.0;
      if ((i == 0 || i == num_intervalos_x_y) && (j == 0 || j == num_intervalos_x_y))
      {
        weight = 0.25; // Canto
      }
      else if (i == 0 || i == num_intervalos_x_y || j == 0 || j == num_intervalos_x_y)
      {
        weight = 0.5; // Borda
      }

      // Soma a contribuição da função ponderada
      integral += weight * f(x_i, y_j);
    }
  }

  // Multiplicação final
  integral *= delta_x * delta_y;
  printf("O valor aproximado para a integral e: %lf\n", integral);

  return 0;
}