#include <iostream>
#include <vector>
#include <ctime>

int main() {
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

    timespec start, stop;
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime"); }

    // Matrix multiplication: C = A * B
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < n; ++k)
                C[i][j] += A[i][k] * B[k][j];

    if (clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror("clock gettime"); }
    double time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1e9;

    std::cout << "Number of FLOPs = " << 2L * n * n * n << ", Execution time = " << time
              << " sec,\n" << (1.0 / time / 1e6 * 2 * n * n * n) << " MFLOPs per sec\n";
    std::cout << "C[100][100]=" << C[100][100] << std::endl;

    return 0;
}