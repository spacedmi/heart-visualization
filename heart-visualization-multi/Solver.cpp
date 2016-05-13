#include "Solver.h"
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <vector>

Solver::Solver(MyHeart* _ode, double _dt, double _maxT, int _countDtTillSave, int _outPutMode) {
	ode = _ode;
	dt = _dt;
	maxT = _maxT;
	countDtTillSave = _countDtTillSave;
    outPutMode = _outPutMode;
}

Solver::~Solver() {
}

void Solver::MultiIntegrate()
{
    if (!ode->SetUp(outPutMode))
    {
		printf("Can't set up ODE\n");
		return;
	}

    DataProcessor dataProcessor(ode);

    int ProcNum, ProcRank, NumOfVertices, numberOfSnapshot;

    // MPI_Scatterv data
    double* ScatterSendBuf;
    int* ScatterSendCounts;
    int ScatterSendCount;
    int* ScatterDispls;
    int* ScatterSendMap;
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

    CellVector = dataProcessor.GetCellVectorByNum(ProcRank);

    // Prepare data for MPI_Gatherv
    ProcOfVertVector = dataProcessor.GetProcOfVertVector();
    NumOfVertices = dataProcessor.GetNumOfVertices();

    GatherRecvCounts = dataProcessor.FillRecvCounts(ProcNum, ProcOfVertVector);
    GatherDispls = dataProcessor.FillDispls(ProcNum, GatherRecvCounts);
    GatherRBuf = new double[NumOfVertices * 2];
    GatherSendCount = CellVector.size() * 2;
    GatherSendBuf = new double[GatherSendCount];

    // Prepare data for MPI_Scatterv
    ScatterSendCount = 2 * dataProcessor.GetTotalCountOfBadNeighbors(ProcNum, ProcOfVertVector);
    ScatterSendBuf = new double[ScatterSendCount];
    ScatterSendCounts = dataProcessor.FillSendCounts(ProcNum, ProcOfVertVector);
    ScatterDispls = dataProcessor.FillDispls(ProcNum, ScatterSendCounts);
    ScatterSendMap = dataProcessor.FillSendMap(ScatterSendCount, ProcNum, ProcOfVertVector);
    ScatterRecvCount = dataProcessor.GetCountOfBadNeighborsByProcRank(ProcRank, ProcOfVertVector) * 2;

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
            if(!dataProcessor.FillScatterSendBuf(ScatterSendBuf, ScatterSendCount, ScatterSendMap))
                return;

        MPI_Scatterv(ScatterSendBuf, ScatterSendCounts, ScatterDispls, MPI_DOUBLE, ScatterRecvBuf,
                     ScatterRecvCount, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (ProcRank != 0)
            if(!dataProcessor.GetInfoFromScatterRecvBuf(ScatterRecvBuf, ScatterRecvCount, ProcRank, ScatterDispls, ScatterSendMap))
                return;

        ode->Step(dt, CellVector);

        // Fill data for MPI_Gatherv
        if(!dataProcessor.FillGatherSendBuf(GatherSendBuf, GatherSendCount, ProcRank, ProcOfVertVector, CellVector))
            return;

        MPI_Gatherv(GatherSendBuf, GatherSendCount, MPI_DOUBLE, GatherRBuf, GatherRecvCounts,
                    GatherDispls, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (ProcRank == 0)
        {
            if(!dataProcessor.GetInfoFromGatherRBuf(GatherRBuf, NumOfVertices * 2, ProcNum, ProcOfVertVector))
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
