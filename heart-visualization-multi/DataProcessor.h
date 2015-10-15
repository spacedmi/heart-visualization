#include "MyHeart.h"
#include <vector>

#define PART_FILE_NAME "input/PartFile.txt"

class DataProcessor
{
public:
    DataProcessor(MyHeart* _ode);

    std::vector<int> GetCellVectorByNum(int currentProcNum);
    std::vector<int> GetProcOfVertVector();
    int* FillRecvCounts(int ProcNum, std::vector<int> ProcOfVertVector);
    int* FillDispls(int ProcNum, int* SendCounts);
    int GetNumOfVertices();
    int GetTotalCountOfBadNeighbors(int currentProc, std::vector<int> ProcOfVertVector);
    bool IsCurNeighborInCurProc(int numOfNeighbor, int currentProc, std::vector<int> ProcOfVertVector);
    int* FillSendCounts(int ProcNum, std::vector<int> ProcOfVertVector);
    int GetCountOfBadNeighborsByProcRank(int currentProc, std::vector<int> ProcOfVertVector);

    int FillScatterSendBuf(double* SendBuf, int SendCount, int* SendMap);
    int GetInfoFromScatterRecvBuf(double* RecvBuf, int  RecvCount, int currentProc, int* Displs, int* SendMap);
    int* FillSendMap(int SendCount, int procNum, std::vector<int> ProcOfVertVector);

    int FillGatherSendBuf(double* SendBuf, int SendCount, int currentProc, std::vector<int> ProcOfVertVector,
                          std::vector<int> CellVector);
    int GetInfoFromGatherRBuf(double* RBuf, int RCount, int procNum, std::vector<int> ProcOfVertVector);
private:
    MyHeart *ode;
};
