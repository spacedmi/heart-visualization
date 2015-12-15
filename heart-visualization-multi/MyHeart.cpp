#include "MyHeart.h"
#include <stdio.h>
#include <math.h>
#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"

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

void MyHeart::Step(double dt, std::vector<int> cellVector) {
	if (!isValid) {
		printf("Heart is not valid\n");
		return;
	}

    double u, v; int currentIndex;
    for (int i = 0; i < cellVector.size(); i++) {
        currentIndex = cellVector[i];
        u = cells[currentIndex].u;
        v = cells[currentIndex].v;
        cells[currentIndex].u1 = u + dt*(u - u*u*u / 3.0 - v + Relation(cells[currentIndex]));
        cells[currentIndex].v1 = v + dt*(u - cells[currentIndex].a)*cells[currentIndex].e;
	}

    for (int i = 0; i < cellVector.size(); i++) {
        currentIndex = cellVector[i];
        cells[currentIndex].u = cells[currentIndex].u1;
        cells[currentIndex].v = cells[currentIndex].v1;
	}
}

double MyHeart::Relation(Cell a) {
	double res = 0.0;
	Cell b;

    for (int i = 0; i < a.countOfNeighbors; i++) {
		b = cells[a.neighbors[i]];
        res += (b.u - a.u) /** Distance(a, b) / a.R*/;
	}

    return res / 4;
}

void MyHeart::SaveState(int numberOfSnapshot) {
	if (!isValid) {
		printf("Heart is not valid\n");
		return;
	}

    snprintf(snapshotFileName, 52, "%s%d.csv", "result/result", numberOfSnapshot);

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

    // TODO: saving to VTK file
    SaveStateToVTK();
}

void MyHeart::SaveStateToVTK()
{
//    //vtkPolyData *data = vtkPolyData::New();
//    vtkUnstructuredGrid *mesh = vtkUnstructuredGrid::New();
//    vtkPoints *points = vtkPoints::New();
//    vtkCellArray *cells = vtkCellArray::New();

//    //vtkIdType pointIDs[4];

//    std::vector<point>::const_iterator iterator1 = fpoints.begin();

//    point aux;

//    if ( event_report != NULL ) { event_report->SetMaxTicks(fpoints.size() + fnodes.size()); }
//    points->SetNumberOfPoints(fpoints.size());
//    int pointId = 0;
//    for (;iterator1 != fpoints.end();iterator1++)
//    {
//        aux = *iterator1;
//        points->SetPoint(pointId, aux.coord);
//        pointId++;
//    }

//    std::vector<node>::const_iterator iterator2 = fnodes.begin();

//    node aux1;

//    vtkSmartPointer<vtkIdTypeArray> idCells =
//      vtkSmartPointer<vtkIdTypeArray>::New();
//    idCells->SetNumberOfComponents(5);
//    idCells->SetNumberOfTuples(fnodes.size());

//    int cellIndex = 0;
//    for (;iterator2 != fnodes.end(); iterator2++)
//    {
//        aux1 = *iterator2;
//        vtkIdType * tuple = new vtkIdType[4];
//        tuple[0] = 4;
//        tuple[1] = aux1.indexs[0] - 1;
//        tuple[2] = aux1.indexs[1] - 1;
//        tuple[3] = aux1.indexs[2] - 1;
//        tuple[4] = aux1.indexs[3] - 1;
//        idCells->SetTupleValue(cellIndex, tuple);
//        cellIndex++;
//    }
//    cells->SetCells(fnodes.size(), idCells);

//    mesh->SetPoints(points);
//    mesh->SetCells(VTK_TETRA, cells);

//    //vtkPolyDataWriter *writer = vtkPolyDataWriter::New();
//    vtkUnstructuredGridWriter *tetra_writer = vtkUnstructuredGridWriter::New();
//    tetra_writer->SetFileName( filename );

//    #if VTK_MAJOR_VERSION <= 5
//        tetra_writer->SetInput(mesh);
//    #else
//        tetra_writer->SetInputData(mesh);
//    #endif

//    tetra_writer->Write();
//    tetra_writer->Delete( );
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

	return true;
}

double MyHeart::sqrDistance(Cell a, Cell b) {
	return ((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z));
}
double MyHeart::Distance(Cell a, Cell b) {
	return sqrt(sqrDistance(a, b));
}
