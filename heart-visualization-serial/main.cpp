#include "Solver.h"
#include "MyHeart.h"
#include <stdlib.h>

#define D_dt 0.005
#define D_count_dt_till_save 200

int main(int argc, char *argv[]) {

    double D_maxT = 100.0;
    int outPutMode = 0;

    if (argc > 2)
    {
        D_maxT = atof(argv[1]);
        outPutMode = atof(argv[2]);
    }
    else
    {
        printf("WARNING: Program needs 2 arguments: computing time and output mode (from 0 to 2)! Starting with default parameters...\n");
    }

    if ((outPutMode < 0) && (outPutMode > 2))
        outPutMode = 0; // 0 - csv, 1 - vtk, 2 - txt bin node state

    MyHeart* ode = new MyHeart();
    Solver solver(ode, D_dt, D_maxT, D_count_dt_till_save, outPutMode);
    solver.Integrate();
    return 0;
}
