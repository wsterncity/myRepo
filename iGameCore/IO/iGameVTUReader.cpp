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
#include "iGameBase64Util.h"

#include <tinyxml2.h>

IGAME_NAMESPACE_BEGIN
bool iGame::iGameVTUReader::Parsing() {
    tinyxml2::XMLElement* elem;
    const char* data;
    const char* attribute;
    char* token;
    /*
     *  used in binary encoded files, if true, the header presents a unsigned long long type number as the total byte num of the binary part.
     *  Otherwise, the header presents a unsigned int type number.
     * */
    attribute = root->Attribute("header_type");
    if(attribute){
        if(strcmp(attribute, "UInt64") == 0){
            is_header_type_uint64 = true;
        }
    }

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
    elem = FindTargetItem(elem, "Points")->FirstChildElement("DataArray");
    data = elem->GetText();
    attribute = elem->Attribute("format");
    if(data)
    {
        Points::Pointer dataSetPoints = m_Data.GetPoints();
        char* data_p = const_cast<char*>(data);
        while (*data_p == '\n' || *data_p == ' ' || *data_p == '\t') data_p ++;
        if(strcmp(attribute, "ascii") == 0){
            float p[3] = { 0 };
            token = strtok(data_p, " ");

            while (token != nullptr) {
                for(float & i : p) {
                    i = mAtof(token);
                    token = strtok(nullptr, " ");
                }
                dataSetPoints->AddPoint(p);
            }
        } else if(strcmp(attribute, "binary") == 0){
            ReadBase64EncodedPoints(data_p, dataSetPoints);
        }
    }

    // find Points' Scalar Data
    elem = FindTargetItem(root, "PointData")->FirstChildElement("DataArray");
    //  use while loop to find point's multiple scala data.
    while(elem){

        data = elem->Attribute("Name");
        std::string scalarName = data ? data :  "Undefined Scalar";
        data = elem->Attribute("NumberOfComponents");

        ArrayObject::Pointer  array;
        const char* type = elem->Attribute("type");

        int scalarComponents = data ? mAtoi(data) : 1;
        data = elem->GetText();
        if(data)
        {
            char* data_p = const_cast<char*>(data);
            while (*data_p == '\n' || *data_p == ' ' || *data_p == '\t') data_p ++;
            attribute = elem->Attribute("format");
            if(!strncmp(type, "Float", 5)){
                if(strcmp(attribute, "binary") == 0){
                    //  Float32
                    if(!strncmp(type + 5, "32", 2)){
                        FloatArray::Pointer arr = FloatArray::New();
                        ReadBase64EncodedArray<float>(data_p, arr);
                        array = arr;
                    } else /*Float64*/{
                        DoubleArray::Pointer arr = DoubleArray::New();
                        ReadBase64EncodedArray<double>(data_p, arr);
                        array = arr;
                    }

                } else if(strcmp(attribute, "ascii") == 0){
                    FloatArray::Pointer arr = FloatArray::New();
                    float* ps = new float[scalarComponents];
                    token = strtok(data_p, " ");
                    while (token != nullptr) {
                        for(int i = 0; i < scalarComponents; i ++) {

                            auto& it = ps[i];
                            it = mAtof(token);
                            token = strtok(nullptr, " ");
                        }
                        arr->AddElement(ps);
                    }
                    array = arr;
                    delete[] ps;
                }
            } else if(!strncmp(type, "Int", 3)){
                if(strcmp(attribute, "binary") == 0){
                    //  Int32
                    if(!strncmp(type + 3, "32", 2)){
                        IntArray::Pointer arr = IntArray::New();
                        ReadBase64EncodedArray<int>(data_p, arr);
                        array = arr;
                    } else /* Int64*/{
                        LongLongArray::Pointer arr = LongLongArray::New();
                        ReadBase64EncodedArray<int64_t>(data_p, arr);
                        array = arr;
                    }
                } else if(strcmp(attribute, "ascii") == 0){
                    IntArray::Pointer arr = IntArray::New();
                    int* ps = new int[scalarComponents];
                    token = strtok(data_p, " ");
                    while (token != nullptr) {
                        for(int i = 0; i < scalarComponents; i ++) {

                            auto& it = ps[i];
                            it = mAtoi(token);
                            token = strtok(nullptr, " ");
                        }
                        arr->AddElement(ps);
                    }
                    array = arr;
                    delete[] ps;
                }
            }
            if(array != nullptr){
                array->SetName(scalarName);
                m_Data.GetData()->AddScalar(IG_POINT, array);
            }
        }
        elem = elem->NextSiblingElement("DataArray");
    }

    // find Piece's Cell data.
    elem = FindTargetItem(root, "Cells");

    //   find Cell connectivity;
    ArrayObject::Pointer CellConnects;

    elem = FindTargetAttributeItem(elem, "DataArray", "Name", "connectivity");
    if(elem)
    {
        data = elem->GetText();

        char* data_p = const_cast<char*>(data);
        while (*data_p == '\n' || *data_p == ' ' || *data_p == '\t') data_p ++;

        attribute = elem->Attribute("format");
        if(strcmp(attribute, "ascii") == 0){
            LongLongArray::Pointer arr = LongLongArray::New();
            token = strtok(data_p, " ");
            int conn = -1;
            while(token)
            {
                conn = mAtoi(token);
                arr->AddValue(conn);
                token = strtok(nullptr, " ");
            }
            CellConnects = arr;
        } else if(strcmp(attribute, "binary") == 0){
            attribute = elem->Attribute("type");
            //  Int32
            if(!strncmp(attribute, "Int32", 5)){
                IntArray::Pointer arr = IntArray::New();
                ReadBase64EncodedArray<int>(data_p, arr);
                CellConnects = arr;
            }else /* Int64*/{
                LongLongArray::Pointer arr = LongLongArray::New();
                ReadBase64EncodedArray<int64_t>(data_p, arr);
                CellConnects = arr;
            }
        }

    }
    //   find Cell offsets;
    ArrayObject::Pointer CellOffsets;
    //  Note that it need to add a zero index.

    elem = FindTargetAttributeItem(elem, "DataArray", "Name", "offsets");
    if(elem)
    {
        data = elem->GetText();
        char* data_p = const_cast<char*>(data);
        while (*data_p == '\n' || *data_p == ' ' || *data_p == '\t') data_p ++;

        attribute = elem->Attribute("format");
        if(strcmp(attribute, "ascii") == 0){
            LongLongArray::Pointer arr = LongLongArray::New();
            arr->AddValue(0);
            int offset = -1;
            token = strtok(data_p, " ");
            while(token)
            {
                offset = mAtoi(token);
                arr->AddValue(offset);
                token = strtok(nullptr, " ");
            }
            CellOffsets = arr;
        } else if(strcmp(attribute, "binary") == 0){
            attribute = elem->Attribute("type");
            //  Int32
            if(!strncmp(attribute, "Int32", 5)){
                IntArray::Pointer arr = IntArray::New();
                arr->AddValue(0);
                ReadBase64EncodedArray<int>(data_p, arr);
                CellOffsets = arr;
            }else /* Int64*/{
                LongLongArray::Pointer arr = LongLongArray::New();
                arr->AddValue(0);
                ReadBase64EncodedArray<int64_t>(data_p, arr);
                CellOffsets = arr;
            }
        }
    }

    //   find Cell types;
    UnsignedCharArray::Pointer CellTypes = UnsignedCharArray::New();
    elem = FindTargetAttributeItem(elem, "DataArray", "Name", "types");
    if(elem)
    {
        data = elem->GetText();
        char* data_p = const_cast<char*>(data);
        while (*data_p == '\n' || *data_p == ' ' || *data_p == '\t') data_p ++;

        attribute = elem->Attribute("format");
        if(strcmp(attribute, "ascii") == 0){
            int type = -1;
            token = strtok(data_p, " ");
            while(token)
            {
                type = mAtoi(token);
                CellTypes->AddValue(type);
                token = strtok(nullptr, " ");
            }
        } else if(strcmp(attribute, "binary") == 0){
            ReadBase64EncodedArray<uint8_t>(data_p, CellTypes);
        }
    }

    VTKAbstractReader::TransferVtkCellToiGameCell(m_Data, CellOffsets, CellConnects, CellTypes);

    return true;
}

IGAME_NAMESPACE_END