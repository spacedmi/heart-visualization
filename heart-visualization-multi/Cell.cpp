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
        a = -1.1; e = 0.01;
        //u0 = -1.1; v0 = -0.656333;
        break;
    case 1:
        a = 0; e = 0.01;
        //u0 = -1.1; v0 = -1.656333;
        break;
    }
	u = u0;
	v = v0;
}
