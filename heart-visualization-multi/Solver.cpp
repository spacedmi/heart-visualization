#include "Solver.h"
#include <stdio.h>
#include <time.h>
#include <fstream>

Solver::Solver(MyHeart* _ode, double _dt, double _maxT, int _countDtTillSave) {
	ode = _ode;
	dt = _dt;
	maxT = _maxT;
	countDtTillSave = _countDtTillSave;
}

Solver::~Solver() {
}

void Solver::MultiIntegrate()
{
    if (!ode->SetUp())
    {
		printf("Can't set up ODE\n");
		return;
	}

    int ProcNum, ProcRank, NumOfVertices, numberOfSnapshot;

    // MPI_Scatterv data
    double* ScatterSendBuf;
    int* ScatterSendCounts;
    int ScatterSendCount;
    int* ScatterDispls;
    double* ScatterRecvBuf;
    int ScatterRecvCount;

    // MPI_Gatherv data
    int GatherSendCount;
    int* GatherRecvCounts;
    int* GatherDispls;
    double* GatherSendBuf;
    double* GatherRBuf;

    double timeToSave;
    time_t work_time;
    std::vector<int> CellVector, ProcOfVertVector;

    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    if (ProcRank == 0)
    {
        work_time = clock();

        numberOfSnapshot = 0;
        ode->SaveState(numberOfSnapshot++);
        timeToSave = 0.0;
        printf("Started counting\n");
    }

    CellVector = GetCellVectorByNum(ProcRank);

    // Prepare data for MPI_Gatherv
    ProcOfVertVector = GetProcOfVertVector();
    NumOfVertices = GetNumOfVertices();

    GatherRecvCounts = FillRecvCounts(ProcNum, ProcOfVertVector);
    GatherDispls = FillDispls(ProcNum, GatherRecvCounts);
    GatherRBuf = new double[NumOfVertices * 2];
    GatherSendCount = CellVector.size() * 2;
    GatherSendBuf = new double[GatherSendCount];

    // Prepare data for MPI_Scatterv
    ScatterSendCount = 2 * GetTotalCountOfBadNeighbors(ProcNum, ProcOfVertVector);
    ScatterSendBuf = new double[ScatterSendCount];
    ScatterSendCounts = FillSendCounts(ProcNum, ProcOfVertVector);
    ScatterDispls = FillDispls(ProcNum, ScatterSendCounts);

    ScatterRecvCount = GetCountOfBadNeighborsByProcRank(ProcRank, ProcOfVertVector) * 2;
    if (ScatterRecvCount <= 0)
    {
        std::cout << "Error: Scatter Recv Count is " << ScatterRecvCount << std::endl;
        return;
    }

    ScatterRecvBuf = new double[ScatterRecvCount];

    for (double t = 0.0; t < maxT; t += dt)
    {
        // Fill data for MPI_Scatterv
        if (ProcRank == 0)
        {
            if(!FillScatterSendBuf(ScatterSendBuf, ScatterSendCount, ProcNum, ProcOfVertVector))
                return;
        }

        MPI_Scatterv(ScatterSendBuf, ScatterSendCounts, ScatterDispls, MPI_DOUBLE, ScatterRecvBuf,
                     ScatterRecvCount, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if(!GetInfoFromScatterRecvBuf(ScatterRecvBuf, ScatterRecvCount, ProcRank, ProcOfVertVector, CellVector))
            return;

        ode->Step(dt, CellVector);

        // Fill data for MPI_Gatherv
        if(!FillGatherSendBuf(GatherSendBuf, GatherSendCount, ProcRank, ProcOfVertVector, CellVector))
            return;

        MPI_Gatherv(GatherSendBuf, GatherSendCount, MPI_DOUBLE, GatherRBuf, GatherRecvCounts,
                    GatherDispls, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (ProcRank == 0)
        {
            if(!GetInfoFromGatherRBuf(GatherRBuf, NumOfVertices * 2, ProcNum, ProcOfVertVector))
                return;
            timeToSave += dt;
            if (timeToSave >= countDtTillSave * dt)
            {
                printf("\r%5.1lf%%", 100.0*t/maxT);
                fflush(stdout);
                ode->SaveState(numberOfSnapshot++);
                timeToSave = 0.0;
            }
        }
	}

    if (ProcRank == 0)
    {
        work_time = clock() - work_time;
        printf("\r + Done\n");
        printf("\rWork tick time: %d, work time in sec: %d\n", (int)work_time,
               (int)(work_time / CLOCKS_PER_SEC));
    }

    delete[] GatherRecvCounts;
    delete[] GatherDispls;
    delete[] GatherRBuf;
    delete[] GatherSendBuf;

    delete[] ScatterSendBuf;
    delete[] ScatterSendCounts;
    delete[] ScatterDispls;
    delete[] ScatterRecvBuf;

    MPI_Barrier(MPI_COMM_WORLD);
}

std::vector<int> Solver::GetCellVectorByNum(int currentProcNum)
{
    std::ifstream partFile;
    std::vector<int> cellVector;
    int totalVertNum, procNum, vertNum;

    partFile.open(std::string(PART_FILE_NAME).c_str());

    if(!partFile)
    {
        std::cout << "Cannot open " << PART_FILE_NAME << " file." << std::endl;
        return cellVector;
    }

    partFile >> totalVertNum;

    for(int i = 0; i < totalVertNum; i++)
    {
        partFile >> procNum >> vertNum;
        if(procNum == currentProcNum)
            cellVector.push_back(vertNum);
    }

    partFile.close();

    if (cellVector.size() == 0)
        std::cout << "Cannot find in PartFile.txt info for " << currentProcNum << " process" << std::endl;
    return cellVector;
}

std::vector<int> Solver::GetProcOfVertVector()
{
    std::ifstream partFile;

    int totalVertNum, procNum, vertNum;

    partFile.open(std::string(PART_FILE_NAME).c_str());

    if(!partFile)
    {
        std::cout << "Cannot open " << PART_FILE_NAME << " file." << std::endl;
    }

    partFile >> totalVertNum;

    std::vector<int> cellVector(totalVertNum);

    for(int i = 0; i < totalVertNum; i++)
    {
        partFile >> procNum >> vertNum;
        cellVector[vertNum] = procNum;
    }

    partFile.close();

    return cellVector;
}

int Solver::GetNumOfVertices()
{
    std::ifstream partFile;

    int totalVertNum;

    partFile.open(std::string(PART_FILE_NAME).c_str());

    if(!partFile)
    {
        std::cout << "Cannot open " << PART_FILE_NAME << " file." << std::endl;
        return 0;
    }

    partFile >> totalVertNum;

    partFile.close();

    return totalVertNum;
}

int* Solver::FillRecvCounts(int ProcNum, std::vector<int> ProcOfVertVector)
{
    int* RecvCounts = new int[ProcNum];

    for(int i = 0; i < ProcNum; i++)
        RecvCounts[i] = 0;

    for(int i = 0; i < ProcOfVertVector.size(); i++)
    {
        RecvCounts[ProcOfVertVector[i]]++;
    }

    for(int i = 0; i < ProcNum; i++)
        RecvCounts[i] *= 2;

    return RecvCounts;
}

int* Solver::FillSendCounts(int ProcNum, std::vector<int> ProcOfVertVector)
{
    int* SendCounts = new int[ProcNum];

    for(int i = 0; i < ProcNum; i++)
        SendCounts[i] = GetCountOfBadNeighborsByProcRank(i, ProcOfVertVector) * 2;

    return SendCounts;
}

int* Solver::FillDispls(int ProcNum, int* SendCounts)
{
    int* Displs = new int[ProcNum];
    Displs[0] = 0;

    for(int i = 1; i < ProcNum; i++)
        Displs[i] = Displs[i - 1] + SendCounts[i - 1];

    return Displs;
}

int Solver::GetTotalCountOfBadNeighbors(int procNum, std::vector<int> ProcOfVertVector)
{
    int totalCount = 0;
    //std::vector<int> CellVector;
    for(int k = 0; k < procNum; k++)
    {
        totalCount += GetCountOfBadNeighborsByProcRank(k, ProcOfVertVector);
//        CellVector = GetCellVectorByNum(k);
//        for(int i = 0; i < CellVector.size(); i++)
//        {
//            for(int j = 0; j < ode->cells[CellVector[i]].countOfNeighbors; j++)
//            {
//                if(!IsCurNeighborInCurProc(ode->cells[CellVector[i]].neighbors[j], k, ProcOfVertVector))
//                {
//                    totalCount++;
//                }
//            }
//        }
    }
//        for(int i = 0; i < ode->count; i++)
//            if(ProcOfVertVector[i] == k)
//                for (int j = 0; j < ode->cells[i].countOfNeighbors; j++)
//                    if (!IsCurNeighborInCurProc(ode->cells[i].neighbors[j], k, ProcOfVertVector))
//                        totalCount++;
    return totalCount;
}

int Solver::GetCountOfBadNeighborsByProcRank(int currentProc, std::vector<int> ProcOfVertVector)
{
    std::vector<int> CellVector = GetCellVectorByNum(currentProc);
    int countOfBadNeighbors = 0;

    for(int i = 0; i < CellVector.size(); i++)
    {
        for(int j = 0; j < ode->cells[CellVector[i]].countOfNeighbors; j++)
        {
            if(!IsCurNeighborInCurProc(ode->cells[CellVector[i]].neighbors[j], currentProc, ProcOfVertVector))
            {
                countOfBadNeighbors++;
            }
        }
    }

    return countOfBadNeighbors;
}

bool Solver::IsCurNeighborInCurProc(int numOfNeighbor, int currentProc, std::vector<int> ProcOfVertVector)
{
    if (ProcOfVertVector[numOfNeighbor] != currentProc )
        return false;
    else
        return true;
}

int Solver::FillScatterSendBuf(double* SendBuf, int SendCount, int procNum, std::vector<int> ProcOfVertVector)
{
    int count = 0;
    for(int k = 0; k < procNum; k++)
        for(int i = 0; i < ode->count; i++)
            if(ProcOfVertVector[i] == k)
                for (int j = 0; j < ode->cells[i].countOfNeighbors; j++)
                    if (!IsCurNeighborInCurProc(ode->cells[i].neighbors[j], k, ProcOfVertVector))
                    {
                        if (SendCount <= count)
                        {
                            std::cout << "Cannot fill scatterv send buffer!" << std::endl;
                            return 0;
                        }
                        SendBuf[count++] = ode->cells[ode->cells[i].neighbors[j]].u;
                        SendBuf[count++] = ode->cells[ode->cells[i].neighbors[j]].v;
                    }
    if (SendCount != count)
    {
        std::cout << "Error in FillScatterSendBuf: SendCount = " << SendCount << "RealCount = " << count << std::endl;
        return 0;
    }
    return 1;
}

int Solver::GetInfoFromScatterRecvBuf(double* RecvBuf, int RecvCount, int currentProc, std::vector<int> ProcOfVertVector,std::vector<int> CellVector)
{
    int count = 0;
    for(int i = 0; i < CellVector.size(); i++)
        for(int j = 0; j < ode->cells[CellVector[i]].countOfNeighbors; j++)
            if(!IsCurNeighborInCurProc(ode->cells[CellVector[i]].neighbors[j], currentProc, ProcOfVertVector))
            {
                if (RecvCount <= count)
                {
                    std::cout << "Cannot get info from scatterv recv buffer! " << "Process " << currentProc << std::endl;
                    return 0;
                }
                ode->cells[ode->cells[CellVector[i]].neighbors[j]].u = RecvBuf[count++];
                ode->cells[ode->cells[CellVector[i]].neighbors[j]].v = RecvBuf[count++];
            }
    return 1;
}

int Solver::FillGatherSendBuf(double* SendBuf, int SendCount, int currentProc, std::vector<int> ProcOfVertVector, std::vector<int> CellVector)
{
    int count = 0;
    for(int i = 0; i < CellVector.size(); i++)
    {
        if (SendCount <= count)
        {
            std::cout << "Cannot fill buffer for gatherv! " << "Process " << currentProc << std::endl;
            return 0;
        }
        SendBuf[count++] = ode->cells[CellVector[i]].u;
        SendBuf[count++] = ode->cells[CellVector[i]].v;
    }

    if (SendCount != count)
    {
        std::cout << "Error in FillGatherSendBuf: SendCount = " << SendCount << "RealCount = " << count << std::endl;
        return 0;
    }
    return 1;
}

int Solver::GetInfoFromGatherRBuf(double* RBuf, int RCount, int procNum, std::vector<int> ProcOfVertVector)
{
    int count = 0;
    for(int k = 0; k < procNum; k++)
        for(int i = 0; i < ode->count; i++)
            if(ProcOfVertVector[i] == k)
            {
                if (RCount <= count)
                {
                    std::cout << "Cannot get info from gather rbuf!" << std::endl;
                    return 0;
                }
                ode->cells[i].u = RBuf[count++];
                ode->cells[i].v = RBuf[count++];
            }
    return 1;
}
