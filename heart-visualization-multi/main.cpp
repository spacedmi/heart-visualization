#include "Solver.h"
#include "MyHeart.h"
#include <stdlib.h>

#define D_dt 0.01
#define D_count_dt_till_save 100

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    double D_maxT = 100.0;

    if (argc > 1)
    {
        D_maxT = atof(argv[1]);
    }
    else
    {
        int ProcRank;
        MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

        if (ProcRank == 0)
        {
            printf("WARNING: Program needs 1 argument: computing time! Starting with default D_maxT...\n");
        }
    }

    MyHeart* ode = new MyHeart();

    Solver solver(ode, D_dt, D_maxT, D_count_dt_till_save);

    solver.MultiIntegrate();

    MPI_Finalize();

    return 0;
}
