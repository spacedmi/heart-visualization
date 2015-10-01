#include "mpi.h"
#include <vector>
#include "Cell.h"

#pragma once

#define PART_FILE_NAME "input/PartFile.txt"

class IODE
{
public:
    virtual bool SetUp() = 0;
    virtual void Step(double dt, std::vector<int> cellVector) = 0;
    virtual void SaveState(int numberOfSnapshot) = 0;
    Cell *cells;
    int count;
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
    int* FillRecvCounts(int ProcNum, std::vector<int> ProcOfVertVector);
    int* FillDispls(int ProcNum, int* SendCounts);
    int GetNumOfVertices();
    int GetTotalCountOfBadNeighbors(int currentProc, std::vector<int> ProcOfVertVector);
    bool IsCurNeighborInCurProc(int numOfNeighbor, int currentProc, std::vector<int> ProcOfVertVector);
    int* FillSendCounts(int ProcNum, std::vector<int> ProcOfVertVector);
    int GetCountOfBadNeighborsByProcRank(int currentProc, std::vector<int> ProcOfVertVector);

    int FillScatterSendBuf(double* SendBuf, int SendCount, int procNum, std::vector<int> ProcOfVertVector);
    int GetInfoFromScatterRecvBuf(double* RecvBuf, int RecvCount, int currentProc, std::vector<int> ProcOfVertVector);
};

