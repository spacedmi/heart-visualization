
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
    int vertNum, edjNum = 0;

    if (argc > 2)
        {
            VertFileName = argv[1];
            TetrFileName = argv[2];
        }
        else
        {
            printf("ERROR: Program needs 2 arguments: vertex and tetrahedron source files!");
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
    if (vertNum > 0)
    {
        int tmp[6];
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
                }

                if (!FindEl(VerticesEdj[tmp[4]], tmp[1]))
                {
                    VerticesEdj[tmp[4]].push_back(tmp[1]);
                    VerticesEdj[tmp[1]].push_back(tmp[4]);
                }
            }
            vector<int>::iterator iter;
            iter = VerticesEdj[2].begin();
                while (iter != VerticesEdj[2].end())
                {
                  cout << *iter << endl;
                  iter++;
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
    cout << vertNum << endl << VertFileName;
    return 0;
 }
