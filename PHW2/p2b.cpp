#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath> //for abs()
#include <pthread.h>
#include <cstdlib> //for atoi

#define h  800
#define w  800

#define input_file  "input.raw"
#define output_file "output2.raw"

struct thread_data {
    int start_idx;
    int end_idx;
    std::vector <float> total;
    std::vector <int> freq;
};

std::vector<unsigned char> a;
std::vector<float> mean_values = {0, 65, 100, 125, 190, 255};
std::vector<int> distance_label(h*w);

int p;
std::vector<thread_data> thread_data_array;
pthread_mutex_t mymutex;
pthread_cond_t cv;
int r =0;
int iteration=0;

void* kmeans(void* threadarg){
    thread_data* my_data = static_cast<thread_data*>(threadarg);
    int start_idx = my_data->start_idx;
    int end_idx = my_data->end_idx;

    for (int i=0; i<50; ++i){
        for (int j = start_idx; j<end_idx; ++j){
            float minimum = 256;
            for (int k=0; k<6;++k){ //assign data to corresponding cluster
                float dist = std::abs(mean_values[k]-a[j]);
                if (dist<minimum){
                    distance_label[j] = k;
                    minimum = dist;
                }
            }
        //locally track
        my_data->total[distance_label[j]]+=a[j];
        ++(my_data->freq)[distance_label[j]];
        }
        pthread_mutex_lock(&mymutex);
        int curr_iteration = iteration;
        if (r<p-1){
            r++;
            while (curr_iteration==iteration){
                pthread_cond_wait(&cv, &mymutex);
            }
        }
        else{
            float overall_total[6] = {0,0,0,0,0,0};
            int overall_freq[6] = {0,0,0,0,0,0};
            r = 0;
            //recalculating mean of each cluster
            for (int i=0; i<p; ++i){
                for (int j=0; j<6; ++j){
                    overall_total[j]+=thread_data_array[i].total[j];
                    overall_freq[j]+=thread_data_array[i].freq[j];
                }
            }

            for (int x=0; x<6; ++x){
                if (overall_freq[x]!=0){
                    mean_values[x] = overall_total[x] / overall_freq[x];
                }
            }
            //reinitialize intermediate data
            for (int i=0; i<p; i++){
                thread_data_array[i].total = std::vector<float>(6, 0.0);
                thread_data_array[i].freq = std::vector<int> (6, 0);
            }
            iteration++;
            pthread_cond_broadcast(&cv);
        }
        pthread_mutex_unlock(&mymutex);

    }

    pthread_exit(nullptr);

}

int main(int argc, char** argv) {
    p = std::atoi(argv[1]);
    a = std::vector<unsigned char>(h * w);

    std::ifstream fin(input_file, std::ios::binary);
    if (!fin) {
        std::cout << "can not open file\n";
        return 1;
    }
    fin.read(reinterpret_cast<char*>(a.data()), h * w);
    fin.close();

    std::vector<pthread_t> threads(p);
    thread_data_array = std::vector<thread_data>(p);

    int rc;
    pthread_mutex_init(&mymutex, nullptr);
    pthread_cond_init(&cv, nullptr);

    auto start = std::chrono::high_resolution_clock::now();

    for (int j=0; j<p; ++j){
        thread_data_array[j].start_idx = j*((h*w)/p);
        thread_data_array[j].end_idx = j*((h*w)/p)+((h*w)/p);
        thread_data_array[j].total = std::vector<float>(6, 0.0);
        thread_data_array[j].freq = std::vector<int> (6, 0);
        rc = pthread_create(&threads[j], nullptr, kmeans, (void*)&thread_data_array[j]);
        if (rc) {
            std::cout << "ERROR" << std::endl;
            exit(-1);
            }
        }
    
    //join threads
    for (int i=0; i<p; ++i){
        pthread_join(threads[i], nullptr);
    }

    //replace each data with mean
    for (int i = 0; i<h*w; ++i){
        a[i] = std::round(mean_values[distance_label[i]]);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " seconds\n";

    std::ofstream fout(output_file, std::ios::binary);
    if (!fout) {
        std::cout << "can not open file\n";
        return 1;
    }
    fout.write(reinterpret_cast<char*>(a.data()), h * w);
    fout.close();

    pthread_mutex_destroy(&mymutex);
    pthread_cond_destroy(&cv);

    return 0;
}