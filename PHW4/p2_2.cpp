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
    
    int arr[64];
    int start = myrank*16;
    int end = start+15;   
    int mysum = 0;

    if (myrank==0){
        ifstream numberFile ("number.txt");

        for (int i =0; i<64; i++){
            numberFile >> arr[i];
        }
        numberFile.close();
    }
    MPI_Bcast(arr, 64, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = start; i<end+1; i++){
            mysum+=arr[i];
        }
    int totalsum = 0;
    MPI_Reduce(&mysum, &totalsum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (myrank==0){
        cout << "Approach 2 - Process " << myrank << ": Sum = " << totalsum << endl;
    }
    
    MPI_Finalize();

    return 0;
}