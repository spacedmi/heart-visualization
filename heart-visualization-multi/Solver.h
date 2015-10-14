#include "mpi.h"
#include <vector>
#include "Cell.h"
#include "MyHeart.h"

#pragma once

#define PART_FILE_NAME "input/PartFile.txt"

class Solver
{
public:
    Solver(MyHeart* _ode, double _dt, double _maxT, int _countDtTillSave);
	~Solver();

    void MultiIntegrate();

private:
    MyHeart *ode;
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

    int FillScatterSendBuf(double* SendBuf, int* Displs, int procNum, std::vector<int> ProcOfVertVector);
    int GetInfoFromScatterRecvBuf(double* SendBuf, int SendCount, int currentProc, std::vector<int> ProcOfVertVector,
                                  std::vector<int> CellVector);

    int FillGatherSendBuf(double* SendBuf, int SendCount, int currentProc, std::vector<int> ProcOfVertVector,
                          std::vector<int> CellVector);
    int GetInfoFromGatherRBuf(double* RBuf, int RCount, int procNum, std::vector<int> ProcOfVertVector);
};

