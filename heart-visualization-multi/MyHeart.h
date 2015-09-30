#include "Solver.h"
#include "Cell.h"
#include <vector>

#pragma once

#define param_a -1.1
#define param_e 0.01
#define FILE_CELL_ALL "input/AllCellFile"

class MyHeart : public IODE
{
public:
	MyHeart();
	~MyHeart();

	// IODE
	bool SetUp();
    void Step(double dt, std::vector<int> cellVector);
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

