#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <omp.h>

#define size 2*1024*1024

void swap (int& a, int& b){
    int temp = a;
    a = b;
    b = temp;
}

void partition (int *arr, int s, int e, int & index){
    int random_index = s+(std::rand() % (e-s+1)); //equivalent to rand()%size for the initial partition, as per the lab's guidelines of first randomly picking up an element of m and reorder
    swap(arr[random_index], arr[e]);
    int pivot = arr[e];
    index = s;
    for (int j = s; j<e; j++){
        if (arr[j]<=pivot){
            swap (arr[index], arr[j]);
            index++;
        }
    }
    swap (arr[index], arr[e]);

}
void quickSort(int *array, int start, int end) {
    if (start>=end){
        return;
    } 
    int index = 0;
    partition(array, start, end, index);
    quickSort (array, start, index-1);
    quickSort(array, index+1, end);
    
}

int main(void) {
    int i;
    struct timespec start, stop; 
    double exe_time;
    
    std::srand(std::time(NULL)); 
    
    // C++ style dynamic memory allocation
    int * m = new int[size];

    for(i = 0; i < size; i++) {
        m[i] = size - i;
        // m[i] = std::rand();
    }

    int threads= 2;
	omp_set_num_threads(threads);
    
    if(clock_gettime(CLOCK_REALTIME, &start) == -1) { 
        perror("clock gettime"); 
    }

    int f =0;
    partition(m, 0, size-1, f);

    #pragma omp parallel
	{
		#pragma omp sections
		{
			#pragma omp section
			{
                quickSort(m, 0, f-1);
			}
			#pragma omp section
			{
                quickSort(m, f, size-1);
			}
		}
	}

    if(clock_gettime(CLOCK_REALTIME, &stop) == -1) { 
        perror("clock gettime"); 
    }		

    exe_time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1e9;
    
    for(i = 0; i < 16; i++) {
        std::printf("%u ", m[i]);
    }

    std::printf("\nExecution time = %f sec\n", exe_time);

    // Clean up memory
    delete[] m;

    return 0;
}	