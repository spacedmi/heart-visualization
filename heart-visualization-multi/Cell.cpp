#include "Cell.h"
#include <stdio.h>

Cell::Cell(void)
{
}

Cell::~Cell(void)
{
}

void Cell::ScanCellFromFile(FILE *f) {
    int odeType;
    fscanf(f, "%i %lf %lf %lf %i %lf %lf %lf %i", &number, &x, &y, &z, &odeType, &R, &u0, &v0, &countOfNeighbors);
    //fscanf(f, "%i %lf %lf %lf %i %lf %lf %lf %i", &number, &x, &y, &z, &odeType, &R, &R, &R, &countOfNeighbors);
	neighbors = new int[countOfNeighbors];

    for (int j = 0; j < countOfNeighbors; j++) {
        fscanf(f, "%i", &neighbors[j]);
	}

    switch(odeType)
    {
    case 0:
        a = -1.3; e = 0.01;
        u0 = -1.3; v0 = -0.2677;
        break;
    case 1:
        a = -0.5; e = 0.01;
        u0 = -1.0; v0 = -0.6667;
        break;
    }
	u = u0;
	v = v0;
}
