// Heart's mesh partition
// Using Metis
// Run command example: ./mesh-partition heart.node heart.ele 2 1
// Algorithms:
// 1 - PartGraphRecursive
// 2 - PartGraphKway
// Weighted:
// 0 - unweighted graph
// 1 - weighted graph


#include <iostream>
#include <stdlib.h>
#include "partitioner.h"

int main(int argc, char *argv[])
{
    char *VertFileName, *TetrFileName;
    int cutNum, algorithm = 1, weighted = 0;

    if (argc > 3)
        {
            VertFileName = argv[1];
            TetrFileName = argv[2];
            cutNum = atoi(argv[3]);
            if (argc > 4) algorithm = atoi(argv[4]);
            if (argc > 5) weighted = atoi(argv[5]);
        }
        else
        {
            std::cout << "ERROR: Program needs 3 arguments: vertex, tetrahedron source files," << std::endl
                    << "number of parts to partition the mesh and number of algorithm (1 or 2)!";
            return -1;
        }

    if (mesh_partition(VertFileName, TetrFileName, cutNum, algorithm, weighted) < 0)
        return -1;

    return 0;
 }
