#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
//#include <omp.h>
#include <cmath> //for abs()

#define h  800
#define w  800

#define input_file  "input.raw"
#define output_file "output.raw"

int main(int argc, char** argv) {
    std::vector<unsigned char> a(h * w);

    std::ifstream fin(input_file, std::ios::binary);
    if (!fin) {
        std::cout << "can not open file\n";
        return 1;
    }
    fin.read(reinterpret_cast<char*>(a.data()), h * w);
    fin.close();

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<float> mean_values = {0, 85, 170, 255};
    std::vector<int> distance_label(h*w);
    for (int i =0; i<30; ++i){
        //initialize variables needed to re-calculate mean (step 3)
        float total[4] = {0,0,0,0};
        int freq[4] = {0,0,0,0};
        //compute distance and assign
        for (int j =0; j<h*w; ++j){
            distance_label[j] = 0;
            float minimum = 256; //initialize an initial min
            for (int k =0; k<4; ++k){
                float dist = std::abs(mean_values[k]-a[j]);
                if (dist<minimum){ //find new cluster
                    distance_label[j] = k;
                    minimum = dist;
                }
            }
            //steps to recompute mean
            total[distance_label[j]] += a[j];
            ++freq[distance_label[j]];
        }

        for (int x=0; x<4; ++x){
            if (freq[x]!=0){
                mean_values[x] = total[x] / freq[x];
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