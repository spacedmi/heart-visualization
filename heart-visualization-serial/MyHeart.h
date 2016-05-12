#include "Cell.h"
#include "mesh.hpp"
#pragma once

#define FILE_CELL_ALL "input/AllCellFile"
#define FILE_TETRAHEDRON "input/tetrahedron"
#define FILE_MESH "input/mesh"

class MyHeart
{
public:
	MyHeart();
	~MyHeart();

	// IODE
    bool SetUp(int _outPutMode);
    void Step(double dt);
	void SaveState(int numberOfSnapshot);
    int getElemNumber();
    int getNodeNumber();
    Cell* cells;
    std::vector<std::vector<int> > tetrahedrons;
    int count, tetraCount;

private:
    int outPutMode, stepCount = 0;
	bool isValid;
	bool ScanHeartFromFile();
    char *snapshotFileName;
    Mesh *mesh;
    void SaveStateToCSV(int numberOfSnapshot);
    void SaveStateToVTK(int numberOfSnapshot);
    void SaveStateToBIN(int numberOfSnapshot);

	double Distance(Cell a, Cell b);
	double sqrDistance(Cell a, Cell b);
	double Relation(Cell a);
    double RelationMassAndStiff(Point point);
};

