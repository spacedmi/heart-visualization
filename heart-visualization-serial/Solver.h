#include "Cell.h"
#include "MyHeart.h"

#pragma once

class Solver
{
public:
    Solver(MyHeart* _ode, double _dt, double _maxT, int _countDtTillSave, int _outPutMode);
	~Solver();

    void Integrate();
private:
    MyHeart *ode;
    double dt;
    double maxT;
    int countDtTillSave;
    int outPutMode;
};
