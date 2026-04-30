#include <stdlib.h>
#include <stdio.h>
#include <cublas.h>
#include <time.h>

#define size 1024

__global__ void mat_mult(int *a, int *b, int *c){
	int row, col, i, lc;
    row = blockIdx.x*blockDim.x+threadIdx.x;
    col = blockIdx.y*blockDim.y+threadIdx.y;
    lc = 0;
    for (i=0; i<size; i++){
        lc+= a[row*size+i]*b[i*size+col];
    }
    c[row*size+col] = lc;
}

int main(int argc, char** argv){	
    int nStreams = atoi(argv[1]);
    int *a, *b, *c;
    cudaMallocHost((void**)&a, (sizeof(int))*size*size);          
	cudaMallocHost((void**)&b, (sizeof(int))*size*size);          
    cudaMallocHost((void**)&c, (sizeof(int))*size*size);          	
	
	  for(int i=0; i<size*size; i++){
			a[i]=i/size;
			b[i]=i%size;
            c[i]=0;
  	}

    cudaStream_t stream[16];
    cudaEvent_t startEvent, endEvent;
    cudaEventCreate(&startEvent);
    cudaEventCreate(&endEvent);
    
    for (int i = 0; i<nStreams; i++){
        cudaStreamCreate(&stream[i]);
    }
    const int streamSize = (size/nStreams)*size;
    const int streamBytes = streamSize * sizeof(int);
    int *gpu_a, *gpu_b, *gpu_c;
    cudaMalloc((void**)&gpu_a, sizeof(int)*size*size); 
    cudaMalloc((void**)&gpu_b, sizeof(int)*size*size);
    cudaMalloc((void**)&gpu_c, sizeof(int)*size*size);

    dim3 dimBlock(16,16);
    dim3 dimGrid(64/nStreams, 64);
    
    cudaMemcpy(gpu_b, b, sizeof(int)*size*size, cudaMemcpyHostToDevice);
    cudaEventRecord(startEvent, 0);
    for (int i=0; i<nStreams; i++){
        int offset = i*streamSize;
        cudaMemcpyAsync(&gpu_a[offset], &a[offset],streamBytes, cudaMemcpyHostToDevice, stream[i]);
        mat_mult<<<dimGrid, dimBlock, 0, stream[i]>>>(gpu_a+offset, gpu_b, gpu_c+offset);
        cudaMemcpyAsync(&c[offset], &gpu_c[offset], streamBytes,cudaMemcpyDeviceToHost, stream[i]);
    }


    cudaEventRecord(endEvent, 0);

    float time;
    cudaEventSynchronize(endEvent);
    cudaEventElapsedTime(&time, startEvent, endEvent);

    printf("time is %f ns\n", time*1e6);	 
    
    printf("c[451][451]=%d\n", c[451*size+451]);

    cudaEventDestroy(startEvent);
    cudaEventDestroy(endEvent);
    cudaFreeHost(a);
    cudaFreeHost(b);
    cudaFreeHost(c);
    for (int i=0; i<nStreams; i++){
        cudaStreamDestroy(stream[i]);
    }
    cudaFree(gpu_a);  
    cudaFree(gpu_b);  
    cudaFree(gpu_c);  
    return 0;
}	
