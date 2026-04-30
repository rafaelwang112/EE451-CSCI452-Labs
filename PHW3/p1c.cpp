#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <omp.h>

int main(int argc, char* argv[]) {
    int p = std::atoi(argv[1]);
    int n = 2048;
    std::vector<std::vector<double> > A(n, std::vector<double>(n));
    std::vector<std::vector<double> > B(n, std::vector<double>(n));
    std::vector<std::vector<double> > C(n, std::vector<double>(n, 0.0));
    omp_set_num_threads(p*p);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = i;
            B[i][j] = i + j;
        }
    }

    double temp = 0;
    for (int i =0; i<n; ++i){ //transpose
        for (int j = i+1; j<n; ++j){
            temp = B[i][j];
            B[i][j] = B[j][i];
            B[j][i] = temp;
        }
    }

    timespec start, stop;
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime"); }

    // Matrix multiplication: C = A * B
    #pragma omp parallel for schedule (static) 
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < n; ++k)
                C[i][j] += A[i][k] * B[j][k];

    if (clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror("clock gettime"); }
    double time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1e9;

    std::cout << "Execution time = " << time
              << " sec,\n";
    std::cout << "C[100][100]=" << C[100][100] << std::endl;

    return 0;
}