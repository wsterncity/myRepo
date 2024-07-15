//
// Created by m_ky on 2024/7/14.
//

/**
 * @class   iGameVTUReader
 * @brief   iGameVTUReader's brief
 */

#include "iGameFileReader.h"
#include "iGameVTUReader.h"

#include "iGameVTKAbstractReader.h"

#include <tinyxml.h>

IGAME_NAMESPACE_BEGIN
bool iGame::iGameVTUReader::Parsing() {
    TiXmlElement* elem;
    const char* data;
    char* nextToken;
    char* token;

    // get Piece's point and Cell num.
    int numOfPoints = 0, numOfCells = 0;
    elem = FindTargetItem(root, "Piece");
    if(elem && (data = elem->Attribute("NumberOfPoints"))){
        numOfPoints = mAtoi(data);
    }
    if(elem && (data = elem->Attribute("NumberOfCells"))){
        numOfCells = mAtoi(data);
    }
    //  find Points' position Data
    elem = FindTargetItem(elem, "Points");
    data = elem->FirstChildElement("DataArray")->GetText();

    if(data)
    {
        Points::Pointer dataSetPoints = m_Data.GetPoints();
        float p[3] = { 0 };
        token = strtok_s(const_cast<char*>(data), " ", &nextToken);
        while (token != nullptr) {
            for(float & i : p) {
                i = mAtof(token);
                token = strtok_s(nullptr, " ", &nextToken);
            }
            dataSetPoints->AddPoint(p);
        }
    }

    // find Points' Scalar Data
//    elem = FindTargetItem(root, "PointData")->FirstChildElement("DataArray");
//    //  use while loop to find point's multiple scala data.
//    while(elem){
//
//        data = elem->Attribute("Name");
//        std::string scalarName = data ? data :  "Undefined Scalar";
//        data = elem->Attribute("NumberOfComponents");
//        int scalarComponents = data ? mAtoi(data) : 1;
//        iGamePointData* pointData;
//        iGameFloatArray* ScalarData;
//        data = elem->GetText();
//
//        if(data)
//        {
//
//            if(dataSet->GetPointData() == nullptr)
//            {
//                pointData = iGamePointData::New();
//                dataSet->SetPointData(pointData);
//            }
//            else
//            {
//                pointData = dataSet->GetPointData();
//            }
//            ScalarData = dynamic_cast<iGameFloatArray *>(pointData->GetScalars(scalarName));
//            if(ScalarData == nullptr)
//            {
//                ScalarData = iGameFloatArray::New();
//                ScalarData->SetName(scalarName);
//                ScalarData->SetNumberOfComponents(scalarComponents);
//                pointData->AddScalars(ScalarData);
//            }
//            float i;
//            token = strtok_s(const_cast<char*>(data), " ", &nextToken);
//            while (token != nullptr)
//            {
//                i = mAtof(token);
//                token = strtok_s(nullptr, " ", &nextToken);
//                ScalarData->AddValue(i);
//            }
//
//        }
//        elem = elem->NextSiblingElement("DataArray");
//    }

    // find Piece's Cell data.
    elem = FindTargetItem(root, "Cells");

    //   find Cell connectivity;
    DataArray::Pointer CellConnects = IntArray::New();

//    iGameLongLongArray* CellConnects = iGameLongLongArray::New();
    elem = FindTargetAttributeItem(elem, "DataArray", "Name", std::string("connectivity"));
    if(elem)
    {
        int conn = -1;
        data = elem->GetText();
        token = strtok_s(const_cast<char*>(data), " ", &nextToken);
        while(token)
        {
            conn = mAtoi(token);
            CellConnects->InsertNextValue((float)conn);
            token = strtok_s(nullptr, " ", &nextToken);
        }
    }

    //   find Cell offsets;
    DataArray::Pointer CellOffsets = IntArray::New();
    //  Note that it need to add a zero index.
    CellOffsets->InsertNextValue((float)0);

    elem = FindTargetAttributeItem(elem, "DataArray", "Name", std::string("offsets"));
    if(elem)
    {
        int offset = -1;
        data = elem->GetText();
        token = strtok_s(const_cast<char*>(data), " ", &nextToken);
        while(token)
        {
            offset = mAtoi(token);
            CellOffsets->InsertNextValue((float)offset);
            token = strtok_s(nullptr, " ", &nextToken);
        }
    }

    //   find Cell types;
    DataArray::Pointer CellTypes = IntArray::New();
    elem = FindTargetAttributeItem(elem, "DataArray", "Name", std::string("types"));
    if(elem)
    {
        int type = -1;
        data = elem->GetText();
        token = strtok_s(const_cast<char*>(data), " ", &nextToken);
        while(token)
        {
            type = mAtoi(token);
            CellTypes->InsertNextValue((float)type);
            token = strtok_s(nullptr, " ", &nextToken);
        }
    }

    VTKAbstractReader::TransferVtkCellToiGameCell(m_Data, CellOffsets, CellConnects, CellTypes);

    return true;
}

IGAME_NAMESPACE_END