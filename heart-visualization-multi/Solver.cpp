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

    int ProcNum, ProcRank, numberOfSnapshot;
    double timeToSave;
    time_t work_time;

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

    for (double t = 0.0; t < maxT; t += dt)
    {

        //ode->Step(dt);

        if (ProcRank == 0)
        {
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

    for(int i = 0; i < vertNum; i++)
    {
        partFile >> procNum >> vertNum;
        if(procNum == currentProcNum)
            cellVector.push_back(i);
    }

    partFile.close();
}
