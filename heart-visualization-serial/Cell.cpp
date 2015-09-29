#include "Cell.h"
#include <stdio.h>

Cell::Cell(void)
{
}

Cell::~Cell(void)
{
}

void Cell::ScanCellFromFile(FILE *f) {
    fscanf(f, "%i %lf %lf %lf %lf %lf %lf %i", &number, &x, &y, &z, &R, &u0, &v0, &countOfNeighbors);
	neighbors = new int[countOfNeighbors];

	for (int j = 0; j<countOfNeighbors; j++) {
        fscanf(f, "%i", &neighbors[j]);
	}

	u = u0;
	v = v0;
}
