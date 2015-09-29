#include "MyHeart.h"
#include <stdio.h>
#include <math.h>

MyHeart::MyHeart() {
	isValid = false;
	snapshotFileName = new char[52];
}

MyHeart::~MyHeart() {
	if (snapshotFileName) delete snapshotFileName;
}


bool MyHeart::SetUp() {
	isValid = ScanHeartFromFile();
	return isValid;
}

void MyHeart::Step(double dt) {
	if (!isValid) {
		printf("Heart is not valid\n");
		return;
	}

	double u, v;
	for (int i = 0; i<count; i++) {
		u = cells[i].u;
		v = cells[i].v;
		cells[i].u1 = u + dt*(u - u*u*u / 3.0 - v + Relation(cells[i]));
		cells[i].v1 = v + dt*(u - param_a)*param_e;
	}

	for (int i = 0; i<count; i++) {
		cells[i].u = cells[i].u1;
		cells[i].v = cells[i].v1;
	}
}

double MyHeart::Relation(Cell a) {
	double res = 0.0;
	Cell b;
	for (int i = 0; i<a.countOfNeighbors; i++) {
		b = cells[a.neighbors[i]];
		res += (b.u - a.u) * Distance(a, b) / a.R;
	}
	return res;
}

void MyHeart::SaveState(int numberOfSnapshot) {
	if (!isValid) {
		printf("Heart is not valid\n");
		return;
	}

    snprintf(snapshotFileName, 52, "%s%d.csv\0", "result/result", numberOfSnapshot);

	char delimiter = ',';
    FILE* writer1 = fopen(snapshotFileName, "w+");
	if (writer1 == NULL) {
        printf("Can't open file %s. Please create folder 'result'\n", snapshotFileName);
		return;
	}
	fprintf(writer1, "x%cy%cz%cscalar\n", delimiter, delimiter, delimiter);
	for (int i = 0; i < count; i++) {
		fprintf(writer1, "%f%c%f%c%f%c%f\n", cells[i].x, delimiter, cells[i].y, delimiter, cells[i].z, delimiter, cells[i].u);
	}
	fclose(writer1);
}


bool MyHeart::ScanHeartFromFile() {
    FILE *f = fopen(FILE_CELL_ALL, "rb");
	if (f == NULL) {
		printf(" - Can't find file %s\n", FILE_CELL_ALL);
		return false;
	}

    fscanf(f, "%i", &count);
	cells = new Cell[count];
	for (int i = 0; i<count; i++) {
		cells[i].ScanCellFromFile(f);
	}

	fclose(f);
	printf(" + Scanned file %s\n", FILE_CELL_ALL);

	// Опускаю половину
	/*
	for (int i = 0; i<count; i++) {
		cells[i].u = cells[i].u0;
		cells[i].v = i<count / 2 ? cells[i].v0 : cells[i].v0 - 2;
	}
	*/

	return true;
}

double MyHeart::sqrDistance(Cell a, Cell b) {
	return ((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z));
}
double MyHeart::Distance(Cell a, Cell b) {
	return sqrt(sqrDistance(a, b));
}
