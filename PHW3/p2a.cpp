#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <omp.h>

#define num_of_points 40000000

struct Point {
    double x;
    double y;
};

int main() {
    int num_of_points_in_circle = 0;
    double pi;
    timespec start, stop;
    double time;
    Point* data_point = new Point[num_of_points];
    for (int i = 0; i < num_of_points; i++) {
        data_point[i].x = static_cast<double>(rand()) / RAND_MAX;
        data_point[i].y = static_cast<double>(rand()) / RAND_MAX;
    }

	int threads = 4;
	omp_set_num_threads(threads);

    if (clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime"); }

    #pragma omp parallel for schedule (static) reduction (+:num_of_points_in_circle)
    for (int i = 0; i < num_of_points; i++) {
        if ((data_point[i].x - 0.5) * (data_point[i].x - 0.5) + (data_point[i].y - 0.5) * (data_point[i].y - 0.5) <= 0.25) {
            num_of_points_in_circle++;
        }
    }

    if (clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror("clock gettime"); }
    time = (stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1e9;

    pi = 4.0 * num_of_points_in_circle / num_of_points;
    printf("Estimated pi is %f, execution time = %f sec\n", pi, time);

    delete[] data_point;
	return 0;
}
