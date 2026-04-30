#include <iostream>
#include <vector>
#include <ctime>

int main() {
    int n = 1024;
    timespec start, stop;
    double time;

    // Allocate matrix and vectors using std::vector
    std::vector<std::vector<double>> matrix(n, std::vector<double>(n));
    std::vector<double> vector(n);
    std::vector<double> result(n);

    // Initialize
    for (int i = 0; i < n; ++i) {
        vector[i] = 3;
        result[i] = 0;
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = 2;
        }
    }

    if (clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime"); }

    // matrix * vector
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            result[i] += matrix[i][j] * vector[j];

    if (clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror("clock gettime"); }
    time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1e9;

    std::cout << "Number of FLOPs = " << 2 * n * n
              << ", Execution time = " << time << " sec,\n"
              << 2 * n * n / time / 1e6 << " MFLOPs per sec\n";

    return 0;
}