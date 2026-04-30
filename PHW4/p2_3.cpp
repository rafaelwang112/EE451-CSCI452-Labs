#include <iostream>
#include <fstream>
#include <mpi.h>

using namespace std;
int main(int argc, char *argv[])
{
    int nps, myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nps);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    int mysum = 0;
    int arr[64];
    if (myrank==0){
        ifstream numberFile ("number.txt");

        for (int i =0; i<64; i++){
            numberFile >> arr[i];
        }
        numberFile.close();
    }
    int myarray[16];
    MPI_Scatter(arr, 16, MPI_INT, myarray, 16, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i<16; i++){
            mysum+=myarray[i];
        }
    int partialsums[4];
    MPI_Gather(&mysum, 1, MPI_INT, partialsums, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (myrank==0){
        int totalsum = 0;
        for (int i =0; i<4; i++){
            totalsum +=partialsums[i];
        }
        cout << "Approach 3 - Process " << myrank << ": Sum = " << totalsum << endl;
    }
    MPI_Finalize();

    return 0;
}