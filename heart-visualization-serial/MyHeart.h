#include "Solver.h"
#include "Cell.h"

#pragma once

#define FILE_CELL_ALL "input/AllCellFile"

class MyHeart : public IODE
{
public:
	MyHeart();
	~MyHeart();

	// IODE
	bool SetUp();
	void Step(double dt);
	void SaveState(int numberOfSnapshot);

private:
	bool isValid;
	bool ScanHeartFromFile();
	char *snapshotFileName;

	int count;
	Cell *cells;

	double Distance(Cell a, Cell b);
	double sqrDistance(Cell a, Cell b);
	double Relation(Cell a);
};

