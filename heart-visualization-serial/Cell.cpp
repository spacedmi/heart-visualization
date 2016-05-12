#include "Cell.h"
#include <stdio.h>

Cell::Cell(void)
{
}

Cell::~Cell(void)
{
}

void Cell::ScanCellFromFile(FILE *f) {
    double max = -19.8;
    double min = -25.8;
    int odeType;
    fscanf(f, "%i %lf %lf %lf %i %lf %lf %lf %i", &number, &x, &y, &z, &odeType, &R, &u0, &v0, &countOfNeighbors);
    neighbors = new int[countOfNeighbors];

    for (int j = 0; j < countOfNeighbors; j++) {
        fscanf(f, "%i", &neighbors[j]);
    }

    switch(odeType)
    {
    case 0:
        a = -1.2; e = 0.01;
        u0 = -1.2; v0 = -0.528;
        break;
    case 1:
        a = -0.8; e = 0.01;
        u0 = -1.0; v0 = -0.6667;
        break;
    case 2:
        a = -0.75 + 0.05 * (z - max) / (max - min) ; e = 0.01;
        u0 = -1.0; v0 = -0.6667;
        break;
    case 3:
        a = -1.8; e = 0.01;
        u0 = -1.8; v0 = 0.144;
        break;
    }
    u = u0;
    v = v0;
}
