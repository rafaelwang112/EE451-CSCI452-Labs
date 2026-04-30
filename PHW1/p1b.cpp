#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib> //for atoi

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

    int b = std::atoi(argv[1]);


    timespec start, stop;
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime"); }

    // Matrix multiplication: C = A * B
    for (int i =0; i<n;i+=b){
        for (int j=0; j<n; j+=b){
            for (int k=0; k<n; k+=b){
                for (int u =0; u<b;++u){
                    for (int v=0; v<b;++v){
                        for (int w=0; w<b;++w){
                            C[i+u][j+v]+=A[i+u][k+w]*B[k+w][j+v];
                        }
                    }
                }
            }
        }
    }

    if (clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror("clock gettime"); }
    double time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1e9;

    std::cout << "Number of FLOPs = " << 2L * n * n * n << ", Execution time = " << time
              << " sec,\n" << (1.0 / time / 1e6 * 2 * n * n * n) << " MFLOPs per sec\n";
    std::cout << "C[100][100]=" << C[100][100] << std::endl;

    return 0;
}