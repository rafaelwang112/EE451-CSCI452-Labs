#include <stdlib.h>
#include <stdio.h>
#include <cublas.h>
#include <time.h>

#define size 1024
#define block_s 32

__global__ void mat_mult(int *a, int *b, int *c){
	int row, col, i, j, lc;
    row = blockIdx.x*blockDim.x+threadIdx.x;
    col = blockIdx.y*blockDim.y+threadIdx.y;
    lc = 0;

    __shared__ int As[block_s][block_s]; 
    __shared__ int Bs[block_s][block_s];

    for (i=0; i<size/block_s; i++){
        As[threadIdx.x][threadIdx.y] = a[row*size+(i*blockDim.y+threadIdx.y)];
        Bs[threadIdx.x][threadIdx.y] = b[(blockDim.x*i+threadIdx.x)*size+col];
        __syncthreads();
        for (j=0; j<block_s; j++){
            lc+=As[threadIdx.x][j]*Bs[j][threadIdx.y];
        }
        __syncthreads();
    }
    c[row*size+col] = lc;
}

int main(){		
    int i;
    int *a = (int*)malloc(sizeof(int)*size*size);          
	int *b = (int*)malloc(sizeof(int)*size*size);          
    int *c = (int*)malloc(sizeof(int)*size*size);           	
	
	  for(i=0; i<size*size; i++){
			a[i]=1;
			b[i]=2;
            c[i]=0;
  	}
	int *gpu_a, *gpu_b, *gpu_c;
	cudaMalloc((void**)&gpu_a, sizeof(int)*size*size); 
	cudaMalloc((void**)&gpu_b, sizeof(int)*size*size);
	cudaMalloc((void**)&gpu_c, sizeof(int)*size*size);
	
	struct timespec start, stop; 
	double time;
	
	
	cudaMemcpy(gpu_a, a, sizeof(int)*size*size, cudaMemcpyHostToDevice);
	cudaMemcpy(gpu_b, b, sizeof(int)*size*size, cudaMemcpyHostToDevice);
	
	dim3 dimGrid(32,32);
	dim3 dimBlock(32,32);
	
	if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) { perror( "clock gettime" );}
	mat_mult<<<dimGrid, dimBlock>>>(gpu_a, gpu_b, gpu_c);				
	cudaMemcpy(c, gpu_c, sizeof(int)*size*size, cudaMemcpyDeviceToHost);
	
	if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror( "clock gettime" );}	  
	time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
	printf("time is %f ns\n", time*1e9);	 
	
	printf("c[451][451]=%d\n", c[451*size+451]);

	free(a);
	free(b);
	free(c);
	cudaFree(gpu_a);  
	cudaFree(gpu_b);  
	cudaFree(gpu_c);  
	return 0;
}	
