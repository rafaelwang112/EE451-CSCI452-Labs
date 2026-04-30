#include <iostream>
#include <mpi.h>

using namespace std;
int main(int argc, char *argv[])
{
    int nps, myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nps);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    int dest = (myrank+1)%nps;
    int src = (myrank-1+nps)%nps;
    int Msg = 0;
    if (myrank==0){
        Msg = 451;
        cout << "Process " << myrank << ": Initially Msg = " << Msg << endl;
        MPI_Send (&Msg, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);

        MPI_Recv(&Msg, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cout << "Process " << myrank << ": Received Msg = " << Msg << ". Done!" << endl;

    }
    else{

        MPI_Recv(&Msg, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        Msg++;
        cout << "Process " << myrank << ": Msg = " << Msg << endl;
        
        MPI_Send (&Msg, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();

    return 0;
}
