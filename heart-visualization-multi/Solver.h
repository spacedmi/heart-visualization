#include "mpi.h"
#include "Cell.h"
#include "MyHeart.h"
#include "DataProcessor.h"

#pragma once

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
};

