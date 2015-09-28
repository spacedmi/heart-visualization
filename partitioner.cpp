#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <time.h>
#include <sstream>
#include <metis.h>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
        {
            std::ostringstream stm ;
            stm << n ;
            return stm.str() ;
        }
}
bool FindEl(std::vector<int> Vert, int Elem)
{
    std::vector<int>::iterator iter;
    iter = Vert.begin();
        while (iter != Vert.end())
        {
          if (Elem == *iter)
              return true;
          iter++;
        }
    return false;
}

int mesh_partition(const char* VertFileName, const char* TetrFileName, const int cutNum,
                   const int algorithm = 1, const int weighted = 0)
{
    if (cutNum < 1)
    {
        std::cerr << "Cut number must be wore than 0 " << VertFileName << std::endl;
        return -1;
    }
    if ((algorithm < 1) || (algorithm > 2))
    {
        std::cerr << "Algorithm number must be 1 or 2 " << VertFileName << std::endl;
        return -1;
    }
//    if ((weighted != 0) || (weighted != 1))
//    {
//        cerr << "Weighted parametr must be 0 or 1 " << VertFileName << endl;
//        return -1;
//    }
    int vertNum, edjNum = 0;
    float *Coord;
    int tmp[6];
    std::vector<int>::iterator iter;

    std::ifstream VertFile(VertFileName);
    std::ifstream TetrFile(TetrFileName);

    // Reading number of vertices in mesh
    if (VertFile)
    {
        if(!VertFile.good())
        {
            std::cerr << "Something wrong with " << VertFileName << std::endl;
            return -1;
        }
        VertFile >> vertNum;
    }
    else
    {
        std::cerr << "ERROR: Can't open file " << VertFileName << std::endl;
        return -1;
    }

    if (vertNum <= 0)
    {
        std::cerr << "Number of verticies is 0" << std::endl;
        return -1;
    }

    int itr = 0;
    Coord = new float[vertNum * 3];
    VertFile >> tmp[1] >> tmp[2] >> tmp[3]; // First line of file
    while(!VertFile.eof())                  // For each line of file
    {
        if(!VertFile.good())
        {
         std::cerr << "Something wrong with " << VertFileName << std::endl;
         return -1;
        }

        VertFile >> tmp[0];
        for (int i = 0; i < 3; i++)
        {
            VertFile >> Coord[itr++]; // Recording all vertices coordinates
        }
    }

    // Reading tetrahedrons and filling structures
    std::vector<int> VerticesEdj[vertNum];
    if(TetrFile)
    {
        TetrFile >> tmp[1] >> tmp[1] >> tmp[1]; // First line of file
        while(!TetrFile.eof())                  // For each line of file
        {
            if(!TetrFile.good())
            {
                std::cerr << "Something wrong with " << TetrFileName << std::endl;
                return -1;
            }
            for (int i = 0; i < 6; i++)
                TetrFile >> tmp[i];

            // Inserting new edjes
            for (int i = 1; i < 4; i++)
            if (!FindEl(VerticesEdj[tmp[i]], tmp[i + 1]))
            {
                VerticesEdj[tmp[i]].push_back(tmp[i + 1]);
                VerticesEdj[tmp[i + 1]].push_back(tmp[i]);
                edjNum++;
            }

            if (!FindEl(VerticesEdj[tmp[4]], tmp[1]))
            {
                VerticesEdj[tmp[4]].push_back(tmp[1]);
                VerticesEdj[tmp[1]].push_back(tmp[4]);
                edjNum++;
            }
        }
    }
    else
    {
        std::cerr << "ERROR: Can't open file " << TetrFileName << std::endl;
        return -1;
    }

    // Prepare data for partition
    idx_t nvtxs,   // The number of vertices in the graph
          ncon,    // The number of balancing constraints (the number of weights associated with each vertex)
          *xadj,   // Array of size nvtxs + 1, contains the vertex shift in adjncy
          *vwgt,   // Array of size ncon * vertNum, contains weight of vertices
          *adjncy, // Array of size 2 * edjNum, contains borders for each vertex
          nparts,  // The number of parts to partition the graph
          objval,  // The edge-cut or the total communication volume of the partitioning solution
          *part;   // This is a vector of size nvtxs that upon successful completion stores the partition vector of the graph

    nvtxs = vertNum;
    ncon  = 1;
    nparts = cutNum;
    objval = 1;
    vwgt = NULL;

    xadj = new idx_t[nvtxs + 1];
    part = new idx_t[nvtxs];
    xadj[0] = 1;
    for (int i = 0; i < nvtxs; i++)
    {
        xadj[i + 1] = xadj[i] + VerticesEdj[i].size();
        //cout << xadj[i + 1] << endl;
    }
    adjncy = new idx_t[2 * edjNum];
    int indx = 0;
    for (int i = 0; i < nvtxs; i++)
    {
        iter = VerticesEdj[i].begin();
            while (iter != VerticesEdj[i].end())
            {
                if (indx < 2 * edjNum)
                {
                    adjncy[indx] = *iter;
                    // cout << adjncy[indx] << endl;
                    indx++;
                    iter++;
                }
                else
                {
                    std::cout << "ERROR: adjncy index >= number of edjeces" << std::endl;
                    break;
                }
            }
    }
    if (weighted == 1)
    {
        vwgt = new idx_t[nvtxs];
        for (int i = 0; i < nvtxs; i++)
        {
            if (i < nvtxs / 3)
                vwgt[i] = 10;
            else
                if (i < nvtxs * 2 / 3)
                    vwgt[i] = 5;
                else
                    vwgt[i] = 1;
        }
    }

    unsigned int time = clock();
    int Result;
    switch (algorithm)
    {
        case 1:
            Result = METIS_PartGraphRecursive(&nvtxs, &ncon, xadj, adjncy, vwgt, NULL, NULL, &nparts,
                                                  NULL, NULL, NULL, &objval, part);
            break;
        case 2:
            Result = METIS_PartGraphKway(&nvtxs, &ncon, xadj, adjncy, vwgt, NULL, NULL, &nparts,
                                                  NULL, NULL, NULL, &objval, part);
            break;
        default:
            Result = METIS_PartGraphRecursive(&nvtxs, &ncon, xadj, adjncy, vwgt, NULL, NULL, &nparts,
                                                  NULL, NULL, NULL, &objval, part);
            break;
    }
    time = clock() - time;

    // Graph partition
    switch (Result)
    {
    case METIS_OK:
        std::cout << "Successful partition" << std::endl;
        break;
    case METIS_ERROR_INPUT:
        std::cout << "Metis input error" << std::endl;
        break;
    case METIS_ERROR_MEMORY:
        std::cout << "Metis memory error" << std::endl;
        break;
    case METIS_ERROR:
        std::cout << "Some Metis error" << std::endl;
        break;
    }
    std::cout << "Total " << vertNum << " vertices" << std::endl;

    // Recorfding each part to .XYZ file
    for (int i = 0; i < cutNum; i++)
    {
        std::ofstream outFile;
        std::stringstream ss;
        outFile.open((patch::to_string(i) + std::string("out.xyz")).c_str());
        outFile << vertNum << std::endl;
        int Number = 0;
        for (int j = 0; j < vertNum; j++)
        {
            if (part[j] == i)
            {
                outFile << Number++;
                outFile << " " << Coord[j * 3] << " " << Coord[j * 3 + 1] <<  " " << Coord[j * 3 + 2] << std::endl;
            }
        }
        outFile.close();
        std::cout << i << " Part: " << Number << " vertices" << std::endl;
    }

    // Recording all vertices to file
    std::ofstream outF;
    outF.open(std::string("PartFile.txt").c_str());
    outF << vertNum << std::endl;
    for (int i = 0; i < vertNum; i++)
    {
        outF << part[i] << " " << i << std::endl;
    }
    outF.close();

    // Data capture
    int relationNum = 0, relationClusterNum = 0;
    int *relationClusterNumArr = new int[cutNum];
    for (int i = 0; i < vertNum; i++)
    {
        iter = VerticesEdj[i].begin();
            while (iter != VerticesEdj[i].end())
            {
                if (part[i] != part[*iter])
                {
                    relationNum++;
                    relationClusterNumArr[part[*iter]] = 1;
                }
                iter++;
            }
    }
    for (int i = 0; i < cutNum; i++)
    {
        // Elements of relationClusterNumArr will be null
        for (int j = 0; j < cutNum; j++)
            relationClusterNumArr[j] = 0;

        for (int j = 0; j < vertNum; j++)
        {
            if (part[j] == i)
            {
                iter = VerticesEdj[j].begin();
                    while (iter != VerticesEdj[j].end())
                    {
                        if (part[j] != part[*iter])
                        {
                            relationClusterNumArr[part[*iter]] = 1;
                        }
                        iter++;
                    }
            }
        }

       // Computing relationClusterNum in relationClusterNumArr
       for (int j = 0; j < cutNum; j++)
           if (relationClusterNumArr[j] == 1)
               relationClusterNum++;
    }
    delete[] relationClusterNumArr;
    // Output data capture results
    std::cout << "relationNum = " << relationNum << " relationClusterNum = " << relationClusterNum << std::endl;
    std::cout << "Work time:" << time << std::endl;
}
