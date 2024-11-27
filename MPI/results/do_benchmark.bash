#!/bin/bash

# Arquivo de saída
output_file="benchmark_results.txt"
# Limpa o arquivo anterior (se houver)
echo "Benchmark Results" > $output_file

# Quantidades de cores e intervalos
cores=(1 2 4 8)
intervals_x=(1000 10000 100000)
intervals_y=(1000 10000 100000)

# Loop para cada combinação de cores, intervalos de x e y
for t in "${cores[@]}"; do
  for ix in "${intervals_x[@]}"; do
    for iy in "${intervals_y[@]}"; do
      
      # Mostra a execução atual no terminal
      echo "Executando com $t cores, x = $ix, y = $iy..."
      echo -e "\n\Cores: $t, x: $ix, y: $iy" >> $output_file
      
      for i in {1..5}; do
        real_time=$( { time mpiexec -np "$t" ../exes/trabalho3.exe "$ix" "$iy"; } 2>&1 | grep real )
        echo $real_time >> $output_file
      done
    done
  done
done

echo "Benchmarks concluídos. Resultados salvos em $output_file."
