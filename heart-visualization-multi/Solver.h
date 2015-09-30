#include "mpi.h"
#include <vector>

#pragma once

#define PART_FILE_NAME "input/PartFile.txt"

class IODE
{
public:
    virtual bool SetUp() = 0;
    virtual void Step(double dt, std::vector<int> cellVector) = 0;
    virtual void SaveState(int numberOfSnapshot) = 0;
};

class Solver
{
public:
	Solver(IODE* _ode, double _dt, double _maxT, int _countDtTillSave);
	~Solver();

    void MultiIntegrate();

private:
	IODE *ode;
    double dt;
    double maxT;
    int countDtTillSave;

    std::vector<int> GetCellVectorByNum(int currentProcNum);
    std::vector<int> GetProcOfVertVector();
};

