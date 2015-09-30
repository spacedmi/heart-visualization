#include "Solver.h"
#include <stdio.h>
#include <time.h>
#include <fstream>

Solver::Solver(IODE* _ode, double _dt, double _maxT, int _countDtTillSave) {
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
//    int ScatterSendCount;
//    int* ScatterRecvCounts;
//    int* ScatterDispls;
//    double* ScatterSendBuf;
//    double* ScatterRBuf;

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
    else
    {
        CellVector = GetCellVectorByNum(ProcRank - 1);
    }

    // Prepare data for MPI_Gatherv
    ProcOfVertVector = GetProcOfVertVector();
    NumOfVertices = GetNumOfVertices();

    GatherRecvCounts = FillRecvCounts(ProcNum, ProcOfVertVector);
    GatherDispls = FillDispls(ProcNum, GatherRecvCounts);
    GatherRBuf = new double[NumOfVertices * 2];
    GatherSendCount = CellVector.size() * 2;
    GatherSendBuf = new double[GatherSendCount];

    for (double t = 0.0; t < maxT; t += dt)
    {
        // TODO: send data to all processes
        //ode->Step(dt);

        if (ProcRank == 0)
        {
            // TODO: recv data from all processes
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
    int* SendCounts = new int[ProcNum];

    for(int i; i < ProcNum; i++)
        SendCounts[i] = 0;

    for(int i = 0; i < ProcOfVertVector.size(); i++)
    {
        SendCounts[ProcOfVertVector[i]]++;
    }

    for(int i; i < ProcNum; i++)
        SendCounts[i] *= 2;

    return SendCounts;
}

int* Solver::FillDispls(int ProcNum, int* SendCounts)
{
    int* Displs = new int[ProcNum];
    Displs[0] = 0;

    for(int i = 1; i < ProcNum; i++)
        Displs[i] = Displs[i - 1] + SendCounts[i];

    return Displs;
}
