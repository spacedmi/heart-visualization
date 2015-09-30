#include "Solver.h"
#include <stdio.h>
#include <time.h>

Solver::Solver(IODE* _ode, double _dt, double _maxT, int _countDtTillSave) {
	ode = _ode;
	dt = _dt;
	maxT = _maxT;
	countDtTillSave = _countDtTillSave;
}

Solver::~Solver() {
}

void Solver::Integrate() {
	if (!ode->SetUp()) {
		printf("Can't set up ODE\n");
		return;
	}

    time_t work_time = clock();

	int numberOfSnapshot = 0;
	ode->SaveState(numberOfSnapshot++);
	double timeToSave = 0.0;

	printf("Started counting\n");
	for (double t = 0.0; t<maxT; t += dt) {
		ode->Step(dt);

		timeToSave += dt;
		if (timeToSave >= countDtTillSave * dt) {
			printf("\r%5.1lf%%", 100.0*t/maxT); fflush(stdout);
			ode->SaveState(numberOfSnapshot++);
			timeToSave = 0.0;
		}
	}

    work_time = clock() - work_time;
	printf("\r + Done\n");
    printf("\rWork tick time: %d, work time in sec: %d\n", (int)work_time,
           (int)(work_time / CLOCKS_PER_SEC));
}
