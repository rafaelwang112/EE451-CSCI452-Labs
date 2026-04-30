#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib> //for atoi
#include <omp.h>


int main(int argc, char* argv[]) {
    int n = 2048;
    std::vector<std::vector<double> > A(n, std::vector<double>(n));
    std::vector<std::vector<double> > B(n, std::vector<double>(n));
    std::vector<std::vector<double> > C(n, std::vector<double>(n, 0.0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = i;
            B[i][j] = i + j;
        }
    }

    int p = std::atoi(argv[1]);
    omp_set_num_threads(p*p);



    timespec start, stop;
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime"); }

    // Matrix multiplication: C = A * B
    #pragma omp parallel for schedule (static) 
    for (int i =0; i<n;i+=8){
        for (int j=0; j<n; j+=8){
            for (int k=0; k<n; k+=8){
                for (int u =0; u<8;++u){
                    for (int v=0; v<8;++v){
                        for (int w=0; w<8;++w){
                            C[i+u][j+v]+=A[i+u][k+w]*B[k+w][j+v];
                        }
                    }
                }
            }
        }
    }

    if (clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror("clock gettime"); }
    double time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1e9;

    std::cout << "Execution time = " << time
              << " sec,\n";
    std::cout << "C[100][100]=" << C[100][100] << std::endl;

    return 0;
}