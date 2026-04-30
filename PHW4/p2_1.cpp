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
    ifstream numberFile ("number.txt");

    for (int i =0; i<64; i++){
        numberFile >> arr[i];
    }

    numberFile.close();
    int start = myrank*16;
    int end = start+15;
    int mysum = 0;
    for (int i = start; i<end+1; i++){
            mysum+=arr[i];
        }
    if (myrank==0){
        int totalsum = mysum;
        int othersum = 0;
        for (int i =1; i<nps; i++){
            MPI_Recv(&othersum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            totalsum+=othersum;
        }
        cout << "Approach 1 - Process " << myrank << ": Sum = " << totalsum << endl;
    }
    else{
        int dest = 0;
        MPI_Send (&mysum, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();

    return 0;
}
