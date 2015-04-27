#include <iostream>
#include <metis.h>
#include <fstream>
#include <vector>
#include <string>
#include <time.h>

using namespace std;
bool FindEl(vector<int> Vert, int Elem)
{
    vector<int>::iterator iter;
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
        cerr << "Cut number must be wore than 0 " << VertFileName << endl;
        return -1;
    }
    if ((algorithm < 1) || (algorithm > 2))
    {
        cerr << "Algorithm number must be 1 or 2 " << VertFileName << endl;
        return -1;
    }
    if ((weighted != 0) || (weighted != 1))
    {
        cerr << "Weighted parametr must be 0 or 1 " << VertFileName << endl;
        return -1;
    }
    int vertNum, edjNum = 0;
    float *Coord;
    int tmp[6];
    vector<int>::iterator iter;

    ifstream VertFile(VertFileName);
    ifstream TetrFile(TetrFileName);

    // Reading number of vertices in mesh
    if (VertFile)
    {
        if(!VertFile.good())
        {
            cerr << "Something wrong with " << VertFileName << endl;
            return -1;
        }
        VertFile >> vertNum;
    }
    else
    {
        cerr << "ERROR: Can't open file " << VertFileName << endl;
        return -1;
    }

    if (vertNum <= 0)
    {
        cerr << "Number of verticies is 0" << endl;
        return -1;
    }

    int itr = 0;
    Coord = new float[vertNum * 3];
    VertFile >> tmp[1] >> tmp[2] >> tmp[3]; // First line of file
    while(!VertFile.eof())                  // For each line of file
    {
        if(!VertFile.good())
        {
         cerr << "Something wrong with " << VertFileName << endl;
         return -1;
        }

        VertFile >> tmp[0];
        for (int i = 0; i < 3; i++)
        {
            VertFile >> Coord[itr++]; // Recording all vertices coordinates
        }
    }

    // Reading tetrahedrons and filling structures
    vector<int> VerticesEdj[vertNum];
    if(TetrFile)
    {
        TetrFile >> tmp[1] >> tmp[1] >> tmp[1]; // First line of file
        while(!TetrFile.eof())                  // For each line of file
        {
            if(!TetrFile.good())
            {
                cerr << "Something wrong with " << TetrFileName << endl;
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
        cerr << "ERROR: Can't open file " << TetrFileName << endl;
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
                    cout << "ERROR: adjncy index >= number of edjeces" << endl;
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
        cout << "Successful partition" << endl;
        break;
    case METIS_ERROR_INPUT:
        cout << "Metis input error" << endl;
        break;
    case METIS_ERROR_MEMORY:
        cout << "Metis memory error" << endl;
        break;
    case METIS_ERROR:
        cout << "Some Metis error" << endl;
        break;
    }
    cout << "Total " << vertNum << " vertices" << endl;

    // Recorfding each part to .XYZ file
    for (int i = 0; i < cutNum; i++)
    {
        ofstream outFile;
        outFile.open(to_string(i) + string("out.xyz"), ios_base::out);
        outFile << vertNum << endl;
        int Number = 0;
        for (int j = 0; j < vertNum; j++)
        {
            if (part[j] == i)
            {
                outFile << Number++;
                outFile << (" " + to_string(Coord[j * 3]) + " " + to_string(Coord[j * 3 + 1]) +
                        " " + to_string(Coord[j * 3 + 2])) << endl;
            }
        }
        outFile.close();
        cout << i << " Part: " << Number << " vertices" << endl;
    }

    // Recording all vertices to file
    ofstream outF;
    outF.open(string("PartFile.txt"), ios_base::out);
    outF << vertNum << endl;
    for (int i = 0; i < vertNum; i++)
    {
        outF << part[i] << " " << i << endl;
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
    cout << "relationNum = " << relationNum << " relationClusterNum = " << relationClusterNum << endl;
    cout << "Work time:" << time << endl;
}
