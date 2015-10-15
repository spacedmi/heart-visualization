#include <stdio.h>
#include "DataProcessor.h"
#include <fstream>
#include <iostream>

DataProcessor::DataProcessor(MyHeart *_ode)
{
    ode = _ode;
}

std::vector<int> DataProcessor::GetCellVectorByNum(int currentProcNum)
{
    std::ifstream partFile;
    std::vector<int> cellVector;
    int totalVertNum, procNum, vertNum;

    partFile.open(std::string(PART_FILE_NAME).c_str());

    if(!partFile)
    {
        std::cout << "Cannot open " << PART_FILE_NAME << " file." << std::endl;
        return cellVector;
    }

    partFile >> totalVertNum;

    for(int i = 0; i < totalVertNum; i++)
    {
        partFile >> procNum >> vertNum;
        if(procNum == currentProcNum)
            cellVector.push_back(vertNum);
    }

    partFile.close();

    if (cellVector.size() == 0)
        std::cout << "Cannot find in PartFile.txt info for " << currentProcNum << " process" << std::endl;
    return cellVector;
}

std::vector<int> DataProcessor::GetProcOfVertVector()
{
    std::ifstream partFile;

    int totalVertNum, procNum, vertNum;

    partFile.open(std::string(PART_FILE_NAME).c_str());

    if(!partFile)
    {
        std::cout << "Cannot open " << PART_FILE_NAME << " file." << std::endl;
    }

    partFile >> totalVertNum;

    std::vector<int> cellVector(totalVertNum);

    for(int i = 0; i < totalVertNum; i++)
    {
        partFile >> procNum >> vertNum;
        cellVector[vertNum] = procNum;
    }

    partFile.close();

    return cellVector;
}

int DataProcessor::GetNumOfVertices()
{
    std::ifstream partFile;

    int totalVertNum;

    partFile.open(std::string(PART_FILE_NAME).c_str());

    if(!partFile)
    {
        std::cout << "Cannot open " << PART_FILE_NAME << " file." << std::endl;
        return 0;
    }

    partFile >> totalVertNum;

    partFile.close();

    return totalVertNum;
}

int* DataProcessor::FillRecvCounts(int ProcNum, std::vector<int> ProcOfVertVector)
{
    int* RecvCounts = new int[ProcNum];

    for(int i = 0; i < ProcNum; i++)
        RecvCounts[i] = 0;

    for(int i = 0; i < ProcOfVertVector.size(); i++)
    {
        RecvCounts[ProcOfVertVector[i]]++;
    }

    for(int i = 0; i < ProcNum; i++)
        RecvCounts[i] *= 2;

    return RecvCounts;
}

int* DataProcessor::FillSendCounts(int ProcNum, std::vector<int> ProcOfVertVector)
{
    int* SendCounts = new int[ProcNum];

    for(int i = 0; i < ProcNum; i++)
        SendCounts[i] = GetCountOfBadNeighborsByProcRank(i, ProcOfVertVector) * 2;

    return SendCounts;
}

int* DataProcessor::FillDispls(int ProcNum, int* SendCounts)
{
    int* Displs = new int[ProcNum];
    Displs[0] = 0;

    for(int i = 1; i < ProcNum; i++)
        Displs[i] = Displs[i - 1] + SendCounts[i - 1];

    return Displs;
}

int DataProcessor::GetTotalCountOfBadNeighbors(int procNum, std::vector<int> ProcOfVertVector)
{
    int totalCount = 0;

    for(int k = 0; k < procNum; k++)
    {
        totalCount += GetCountOfBadNeighborsByProcRank(k, ProcOfVertVector);
    }

    return totalCount;
}

int DataProcessor::GetCountOfBadNeighborsByProcRank(int currentProc, std::vector<int> ProcOfVertVector)
{
    std::vector<int> CellVector = GetCellVectorByNum(currentProc);
    int countOfBadNeighbors = 0;

    for(int i = 0; i < CellVector.size(); i++)
    {
        for(int j = 0; j < ode->cells[CellVector[i]].countOfNeighbors; j++)
        {
            if(!IsCurNeighborInCurProc(ode->cells[CellVector[i]].neighbors[j], currentProc, ProcOfVertVector))
            {
                countOfBadNeighbors++;
            }
        }
    }

    return countOfBadNeighbors;
}

bool DataProcessor::IsCurNeighborInCurProc(int numOfNeighbor, int currentProc, std::vector<int> ProcOfVertVector)
{
    if (ProcOfVertVector[numOfNeighbor] != currentProc )
        return false;
    else
        return true;
}

int* DataProcessor::FillSendMap(int SendCount, int procNum, std::vector<int> ProcOfVertVector)
{
    int* SendMap = new int[SendCount];

    int count = 0;
    for(int k = 0; k < procNum; k++)
        for(int i = 0; i < ode->count; i++)
            if(ProcOfVertVector[i] == k)
                for (int j = 0; j < ode->cells[i].countOfNeighbors; j++)
                    if (!IsCurNeighborInCurProc(ode->cells[i].neighbors[j], k, ProcOfVertVector))
                    {
                        if (SendCount <= count)
                        {
                            std::cout << "Cannot fill scatterv send buffer!" << std::endl;
                            return 0;
                        }
                        SendMap[count++] = ode->cells[i].neighbors[j];
                        SendMap[count++] = ode->cells[i].neighbors[j];
                    }

    return SendMap;
}

int DataProcessor::FillScatterSendBuf(double* SendBuf, int SendCount, int* SendMap)
{
    for(int i = 0; i < SendCount / 2; i++)
    {
        SendBuf[i * 2] = ode->cells[SendMap[i * 2]].u;
        SendBuf[i * 2 + 1] = ode->cells[SendMap[i * 2 + 1]].v;
    }
    return 1;
}

int DataProcessor::GetInfoFromScatterRecvBuf(double* RecvBuf, int  RecvCount, int currentProc, int* Displs, int* SendMap)
{
    int count = 0;
    for(int i = 0; i < RecvCount / 2; i++)
    {
        ode->cells[SendMap[Displs[currentProc] + i * 2]].u = RecvBuf[count++];
        ode->cells[SendMap[Displs[currentProc] + i * 2 + 1]].v = RecvBuf[count++];
    }

    if (RecvCount != count)
    {
        std::cout << "Error in GetInfoFromScatterRecvBuf: RecvCount = " << RecvCount << "RealCount = " << count << std::endl;
        return 0;
    }

    return 1;
}

int DataProcessor::FillGatherSendBuf(double* SendBuf, int SendCount, int currentProc, std::vector<int> ProcOfVertVector, std::vector<int> CellVector)
{
    int count = 0;
    for(int i = 0; i < CellVector.size(); i++)
    {
        if (SendCount <= count)
        {
            std::cout << "Cannot fill buffer for gatherv! " << "Process " << currentProc << std::endl;
            return 0;
        }
        SendBuf[count++] = ode->cells[CellVector[i]].u;
        SendBuf[count++] = ode->cells[CellVector[i]].v;
    }

    if (SendCount != count)
    {
        std::cout << "Error in FillGatherSendBuf: SendCount = " << SendCount << "RealCount = " << count << std::endl;
        return 0;
    }
    return 1;
}

int DataProcessor::GetInfoFromGatherRBuf(double* RBuf, int RCount, int procNum, std::vector<int> ProcOfVertVector)
{
    int count = 0;
    for(int k = 0; k < procNum; k++)
        for(int i = 0; i < ode->count; i++)
            if(ProcOfVertVector[i] == k)
            {
                if (RCount <= count)
                {
                    std::cout << "Cannot get info from gather rbuf!" << std::endl;
                    return 0;
                }
                ode->cells[i].u = RBuf[count++];
                ode->cells[i].v = RBuf[count++];
            }
    return 1;
}
