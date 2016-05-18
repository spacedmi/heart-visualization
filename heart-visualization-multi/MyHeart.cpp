#include "MyHeart.h"
#include <stdio.h>
#include <math.h>
#define VTK 0

#ifdef VTK

#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

#endif
MyHeart::MyHeart() {
	isValid = false;
	snapshotFileName = new char[52];
    outPutMode = 0;
}

MyHeart::~MyHeart() {
	if (snapshotFileName) delete snapshotFileName;
}

bool MyHeart::SetUp(int _outPutMode) {
    outPutMode = _outPutMode;
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

    return res / 8;
}

void MyHeart::SaveState(int numberOfSnapshot) {
	if (!isValid) {
		printf("Heart is not valid\n");
		return;
	}

    switch (outPutMode)
    {
    case 0:
        SaveStateToCSV(numberOfSnapshot);
        break;
    case 1:
        SaveStateToVTK(numberOfSnapshot);
        break;
    case 2:
        SaveStateToBIN(numberOfSnapshot);
        break;
    }
}

void MyHeart::SaveStateToCSV(int numberOfSnapshot)
{
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
}

void MyHeart::SaveStateToVTK(int numberOfSnapshot)
{
    #ifdef VTK
        snprintf(snapshotFileName, 52, "%s%d.vtk", "result/VTKresult", numberOfSnapshot);

        vtkUnstructuredGrid *mesh = vtkUnstructuredGrid::New();
        vtkPoints *points = vtkPoints::New();
        vtkCellArray *vtkCells = vtkCellArray::New();

        points->SetNumberOfPoints(count);
        for (int i = 0; i < count; i++)
        {
            points->SetPoint(i, cells[i].x, cells[i].y, cells[i].z);
        }

        vtkSmartPointer<vtkIdTypeArray> idCells =
          vtkSmartPointer<vtkIdTypeArray>::New();
        idCells->SetNumberOfComponents(5);
        idCells->SetNumberOfTuples(tetraCount);

        for (int i = 0; i < tetraCount; i++)
        {
            vtkIdType tuple[4];
            tuple[0] = 4;
            tuple[1] = tetrahedrons[i][0];
            tuple[2] = tetrahedrons[i][1];
            tuple[3] = tetrahedrons[i][2];
            tuple[4] = tetrahedrons[i][3];
            idCells->SetTupleValue(i, tuple);
        }
        vtkCells->SetCells(tetraCount, idCells);

        vtkSmartPointer<vtkFloatArray> scalar=
              vtkSmartPointer<vtkFloatArray>::New();
        scalar->SetNumberOfComponents(1);
        scalar->SetNumberOfValues(count);
        for (int i = 0; i < count; i++)
            scalar->SetValue(i, (float)cells[i].u);

        mesh->SetPoints(points);
        mesh->SetCells(VTK_TETRA, vtkCells);
        mesh->GetPointData()->SetScalars(scalar);

        vtkUnstructuredGridWriter *tetra_writer = vtkUnstructuredGridWriter::New();
        tetra_writer->SetFileName( snapshotFileName );
        tetra_writer->SetFileTypeToBinary();

        #if VTK_MAJOR_VERSION <= 5
            tetra_writer->SetInput(mesh);
        #else
            tetra_writer->SetInputData(mesh);
        #endif

        tetra_writer->Write();
        tetra_writer->Delete( );

        scalar.~vtkSmartPointerBase();
        idCells.~vtkSmartPointerBase();
        mesh->Delete();
        points->Delete();
        vtkCells->Delete();
    #endif
}

void MyHeart::SaveStateToBIN(int numberOfSnapshot)
{
        snprintf(snapshotFileName, 52, "%s%d.txt", "result/result", numberOfSnapshot);

        FILE* writer1 = fopen(snapshotFileName, "w+");
        if (writer1 == NULL) {
            printf("Can't open file %s. Please create folder 'result'\n", snapshotFileName);
            return;
        }

        for (int i = 0; i < count; i++) {
            fprintf(writer1, "%d\n", (cells[i].u < 0) ? 0 : 1);
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

    // read tetrahedron
    FILE *tetrF = fopen(FILE_TETRAHEDRON, "rb");
    if (tetrF == NULL) {
        printf(" - Can't find file %s\n", FILE_TETRAHEDRON);
        return false;
    }

    int temp;
    fscanf(tetrF, "%i", &tetraCount);
    fscanf(tetrF, "%i %i", &temp, &temp);

    tetrahedrons.resize(tetraCount);
      for (int i = 0; i < tetraCount; ++i)
        tetrahedrons[i].resize(4);

    for (int i = 0; i < tetraCount; i++) {
        fscanf(tetrF, "%i", &temp);
        fscanf(tetrF, "%i", &tetrahedrons[i][0]);
        fscanf(tetrF, "%i", &tetrahedrons[i][1]);
        fscanf(tetrF, "%i", &tetrahedrons[i][2]);
        fscanf(tetrF, "%i", &tetrahedrons[i][3]);
    }

    fclose(tetrF);
	return true;
}

double MyHeart::sqrDistance(Cell a, Cell b) {
	return ((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z));
}
double MyHeart::Distance(Cell a, Cell b) {
	return sqrt(sqrDistance(a, b));
}
