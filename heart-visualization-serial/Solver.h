#pragma once

class IODE
{
public:
	virtual bool SetUp() = 0;							// ��������� �������, ���������� bool == ������� ��
	virtual void Step(double dt) = 0;					// ����������� 1 ���
	virtual void SaveState(int numberOfSnapshot) = 0;	// ��������� ��������� ����� 'countDtTillSave' ��������
};

class Solver
{
public:
	Solver(IODE* _ode, double _dt, double _maxT, int _countDtTillSave);
	~Solver();

	void Integrate();		// ��������������

private:
	IODE *ode;
	double dt;				// ��� �� ������� ��� ���������� ��������������
	double maxT;			// �� ������ ������� �����������
	int countDtTillSave;	// ����� ������� dt ���������
};

