#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define A_X 0.0
#define B_X 1.5
#define A_Y 0.0
#define B_Y 1.5
#define THREADS_PER_BLOCK 512

__device__ double f(double x, double y) {
    return sin(x * x + y * y);
}

__global__ void integralKernel(double* result, int nx, int ny, double delta_x, double delta_y) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int total_threads = gridDim.x * blockDim.x;
    double local_sum = 0.0;
    
    // Cada thread processa múltiplos pontos
    for(int i = tid; i <= nx; i += total_threads) {
        double x_i = A_X + i * delta_x;
        
        for(int j = 0; j <= ny; j++) {
            double y_j = A_Y + j * delta_y;
            double weight = 1.0;
            
            if((i == 0 || i == nx) && (j == 0 || j == ny)) {
                weight = 0.25; // Canto
            }
            else if(i == 0 || i == nx || j == 0 || j == ny) {
                weight = 0.5; // Borda
            }
            
            local_sum += weight * f(x_i, y_j);
        }
    }
    
    __shared__ double shared_sum[THREADS_PER_BLOCK];
    shared_sum[threadIdx.x] = local_sum;
    __syncthreads();
    
    for(int stride = blockDim.x/2; stride > 0; stride >>= 1) {
        if(threadIdx.x < stride) {
            shared_sum[threadIdx.x] += shared_sum[threadIdx.x + stride];
        }
        __syncthreads();
    }
    
    if(threadIdx.x == 0) {
        atomicAdd(result, shared_sum[0] * delta_x * delta_y);
    }
}

int main(int argc, char *argv[]) {
    if(argc != 4) {
        fprintf(stderr, "Uso: %s <num_blocks> <nx> <ny>\n", argv[0]);
        return 1;
    }
    
    int num_blocks = atoi(argv[1]);
    int nx = atoi(argv[2]);
    int ny = atoi(argv[3]);
    
    double *d_result, h_result = 0.0;
    const double delta_x = (B_X - A_X) / nx;
    const double delta_y = (B_Y - A_Y) / ny;
    
    cudaMalloc(&d_result, sizeof(double));
    cudaMemcpy(d_result, &h_result, sizeof(double), cudaMemcpyHostToDevice);
    
    integralKernel<<<num_blocks, THREADS_PER_BLOCK>>>(d_result, nx, ny, delta_x, delta_y);
    
    cudaMemcpy(&h_result, d_result, sizeof(double), cudaMemcpyDeviceToHost);
    cudaFree(d_result);
    
    printf("Blocos: %d, Nx: %d, Ny: %d\n", num_blocks, nx, ny);
    printf("Valor aproximado da integral: %.10lf\n", h_result);
    
    return 0;
}