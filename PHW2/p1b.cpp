#include <iostream>
#include <vector>
#include <ctime>
#include <pthread.h>
#include <cstdlib> //for atoi

int n;
std::vector<std::vector<double> > A;
std::vector<std::vector<double> > B;
std::vector<std::vector<double> > C;


struct thread_data {
    int row_idx_start;
    int row_idx_end;
    int col_idx_start;
    int col_idx_end;
};

std::vector<pthread_mutex_t> mymutexes;

void* matrixMultiply(void* threadarg){
    thread_data* my_data = static_cast<thread_data*>(threadarg);
    int row_s = my_data->row_idx_start;
    int row_e = my_data->row_idx_end;
    int col_s = my_data->col_idx_start;
    int col_e = my_data->col_idx_end;
    for (int i = row_s; i < row_e; ++i){
        for (int j = col_s; j < col_e; ++j){
            pthread_mutex_lock(&mymutexes[i]);
            for (int k = 0; k < n; ++k){
                C[i][k] += A[i][j] * B[j][k];
            }
            pthread_mutex_unlock(&mymutexes[i]);
        }

    }
   
    pthread_exit(nullptr);

}

int main(int argc, char* argv[]) {
    n = 4096;
    A = std::vector<std::vector<double> >(n, std::vector<double>(n));
    B= std::vector<std::vector<double> > (n, std::vector<double>(n));
    C= std::vector<std::vector<double> > (n, std::vector<double>(n, 0.0));
    mymutexes = std::vector<pthread_mutex_t> (n);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = i;
            B[i][j] = i + j;
        }
    }

    for (int i =0 ; i<n; ++i){ //one mutex to handle each row
        pthread_mutex_init(&mymutexes[i], nullptr);
    }

    int p = std::atoi(argv[1]);
    int num_threads = p*p;
    std::vector<pthread_t> threads(num_threads);
    std::vector<thread_data> thread_data_array(num_threads);
    int rc;

    timespec start, stop;
    if (clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime"); }

    for (int i = 0; i<p; ++i){
        for (int j = 0; j<p; ++j){
            thread_data_array[i*p+j].row_idx_start = i*(n/p);
            thread_data_array[i*p+j].row_idx_end = i*(n/p)+(n/p);
            thread_data_array[i*p+j].col_idx_start = j*(n/p);
            thread_data_array[i*p+j].col_idx_end = j*(n/p)+(n/p);
            rc = pthread_create(&threads[i*p+j], nullptr, matrixMultiply, (void*)&thread_data_array[i*p+j]);
            if (rc) {
            std::cout << "ERROR" << std::endl;
            exit(-1);
            }
        }
    }

    for (int i = 0; i<p; ++i){
        for (int j = 0; j<p; ++j){
            pthread_join(threads[i*p+j], nullptr);
        }
    }

    if (clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror("clock gettime"); }
    double time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1e9;

    std::cout << "Execution time = " << time
              << " sec\n";
    std::cout << "C[100][100]=" << C[100][100] << std::endl;

    for (int i=0; i<n; ++i){
        pthread_mutex_destroy(&mymutexes[i]);
    }

    return 0;
}