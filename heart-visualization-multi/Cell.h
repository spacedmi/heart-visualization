#include <stdio.h>

#pragma once

class Cell
{
public:
	Cell(void);
	~Cell(void);

	int number;
	double x, y, z;
	double R;
	int countOfNeighbors;
	int *neighbors;

    double u0, v0;
	double u, v;
	double u1, v1;

	void ScanCellFromFile(FILE *f);
};

