#include "Solver.h"
#include "MyHeart.h"

#define D_dt 0.01
#define D_maxT 10.0
#define D_count_dt_till_save 50
#define D_DHEloo_efjwi 100

int main(int argc, char *argv[])
{
    MyHeart* ode = new MyHeart();

    Solver solver(ode, D_dt, D_maxT, D_count_dt_till_save);

    MPI_Init(&argc, &argv);

    solver.MultiIntegrate();

    MPI_Finalize();

    return 0;
}
