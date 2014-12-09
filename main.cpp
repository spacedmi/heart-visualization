
// Heart's mesh partition
// Using Metis

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <metis.h>
#include <fstream>
#include <vector>

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

int main(int argc, char *argv[])
{
    char *VertFileName, *TetrFileName;
    int vertNum, edjNum = 0, cutNum;
    vector<int>::iterator iter;

    if (argc > 3)
        {
            VertFileName = argv[1];
            TetrFileName = argv[2];
            cutNum = atoi(argv[3]);
        }
        else
        {
            cout << "ERROR: Program needs 3 arguments: vertex, tetrahedron source files," << endl
                    << "and number of parts to partition the mesh!";
            return -1;
        }

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

    // Reading tetrahedrons and filling structures
    vector<int> VerticesEdj[vertNum];
    if (vertNum > 0)
    {
        int tmp[6];

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
    }
    else
    {
        cerr << "Number of verticies is 0" << endl;
        return -1;
    }

    // Prepare data for partition
    idx_t nvtxs,   // The number of vertices in the graph
          ncon,    // The number of balancing constraints
          *xadj,   // Array of size nvtxs + 1, contains the vertex shift in adjncy
          *adjncy, // Array of size 2 * edjNum, contains borders for each vertex
          nparts,  // The number of parts to partition the graph
          objval,  // The edge-cut or the total communication volume of the partitioning solution
          *part;   // This is a vector of size nvtxs that upon successful completion stores the partition vector of the graph

    nvtxs = vertNum;
    ncon  = 1;
    nparts = cutNum;
    objval = 1;

    xadj = new idx_t[nvtxs + 1];
    part = new idx_t[nvtxs];
    xadj[0] = 1;
    for (int i = 0; i < vertNum; i++)
    {
        xadj[i + 1] = xadj[i] + VerticesEdj[i].size();
        //cout << xadj[i + 1] << endl;
    }
    adjncy = new idx_t[2 * edjNum];
    int indx = 0;
    for (int i = 0; i < vertNum; i++)
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
    int Result = METIS_PartGraphRecursive(&nvtxs, &ncon, xadj, adjncy, NULL, NULL, NULL, &nparts,
                                          NULL, NULL, NULL, &objval, part);
    // Graph partition
    switch (Result)
    {
    case METIS_OK:
        cout << "Successful partition" << endl;
        for(int i = 0; i < vertNum; i++)
            cout << part[i] << endl;
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
    cout << vertNum << endl << VertFileName;
    return 0;
 }
