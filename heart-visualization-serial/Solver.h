#pragma once

class IODE
{
public:
	virtual bool SetUp() = 0;							// Считываем решётку, возвращаем bool == удалось ли
	virtual void Step(double dt) = 0;					// Интегрируем 1 шаг
	virtual void SaveState(int numberOfSnapshot) = 0;	// Сохраняем состояние после 'countDtTillSave' итераций
};

class Solver
{
public:
	Solver(IODE* _ode, double _dt, double _maxT, int _countDtTillSave);
	~Solver();

	void Integrate();		// Интегрирование

private:
	IODE *ode;
	double dt;				// шаг по времени для численного интегрирования
	double maxT;			// до какого времени интегрируем
	int countDtTillSave;	// через сколько dt сохраняем
};

