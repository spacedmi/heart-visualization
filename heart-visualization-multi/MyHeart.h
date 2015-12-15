#include "Cell.h"
#include <vector>

#pragma once

#define FILE_CELL_ALL "input/AllCellFile"

class MyHeart
{
public:
	MyHeart();
	~MyHeart();

	// IODE
	bool SetUp();
    void Step(double dt, std::vector<int> cellVector);
	void SaveState(int numberOfSnapshot);
    Cell *cells;
    int count;

private:
	bool isValid;
    char *snapshotFileName;
	bool ScanHeartFromFile();
    void SaveStateToVTK();

	double Distance(Cell a, Cell b);
	double sqrDistance(Cell a, Cell b);
	double Relation(Cell a);
};

