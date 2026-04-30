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
#define output_file "output1.raw"

struct thread_data {
    int start_idx;
    int end_idx;
    std::vector <float> total;
    std::vector <int> freq;
};

std::vector<unsigned char> a;
std::vector<float> mean_values = {0, 65, 100, 125, 190, 255};
std::vector<int> distance_label(h*w);

void* kmeans(void* threadarg){
    thread_data* my_data = static_cast<thread_data*>(threadarg);
    int start_idx = my_data->start_idx;
    int end_idx = my_data->end_idx;

    for (int i = start_idx; i<end_idx; ++i){
        float minimum = 256;
        for (int j=0; j<6;++j){ //assign data to corresponding cluster
            float dist = std::abs(mean_values[j]-a[i]);
            if (dist<minimum){
                distance_label[i] = j;
                minimum = dist;
            }
        }
        //locally track
        my_data->total[distance_label[i]]+=a[i];
        ++(my_data->freq)[distance_label[i]];
    }

    pthread_exit(nullptr);

}

int main(int argc, char** argv) {
    int p = std::atoi(argv[1]);
    a = std::vector<unsigned char>(h * w);

    std::ifstream fin(input_file, std::ios::binary);
    if (!fin) {
        std::cout << "can not open file\n";
        return 1;
    }
    fin.read(reinterpret_cast<char*>(a.data()), h * w);
    fin.close();

    std::vector<pthread_t> threads(p);
    std::vector<thread_data> thread_data_array(p);
    int rc;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i =0; i<50; ++i){
        float overall_total[6] = {0,0,0,0,0,0};
        int overall_freq[6] = {0,0,0,0,0,0};
        //spawn threads and distribute
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
        //synchronize threads
        for (int i=0; i<p; ++i){
            pthread_join(threads[i], nullptr);
        }
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

    return 0;
}