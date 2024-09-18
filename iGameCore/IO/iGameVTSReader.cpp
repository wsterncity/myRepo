//
// Created by m_ky on 2024/7/14.
//

/**
 * @class   iGameVTSReader
 * @brief   iGameVTSReader's brief
 */

#include "iGameVTSReader.h"
#include "iGameFileReader.h"
#include "iGameBase64Util.h"

#include <tinyxml2.h>
#undef max
#undef min



IGAME_NAMESPACE_BEGIN
bool iGame::iGameVTSReader::Parsing() {
    tinyxml2::XMLElement* elem;
    const char* data;
    const char* attribute;
    const char* delimiters = " \n";
    char* token;
    // get x y z range.
    elem = FindTargetItem(root, "StructuredGrid");
    int x_dimension, y_dimension, z_dimension;
    if(elem){
        data = elem->Attribute("WholeExtent");
//        char* nextToken;
        token = strtok(const_cast<char*>(data), delimiters);
        int l, r;
        l = mAtoi(token);
        token = strtok(nullptr, delimiters);
        r = mAtoi(token);
        token = strtok(nullptr, delimiters);
        x_dimension = r - l + 1;

        l = mAtoi(token);
        token = strtok(nullptr, delimiters);
        r = mAtoi(token);
        token = strtok(nullptr, delimiters);
        y_dimension = r - l + 1;

        l = mAtoi(token);
        token = strtok(nullptr, delimiters);
        r = mAtoi(token);
        z_dimension = r - l + 1;
    } else {
        printf("Could not load Vts file . Error='No StructuredGrid Attribute'. Exiting.\n");
        return false;
    }
    std::cout << x_dimension << ' ' << y_dimension << ' ' << z_dimension << '\n';
    //  find Points' position Data
    elem = FindTargetItem(root, "Points")->FirstChildElement("DataArray");
    data = elem->GetText();

    Points::Pointer Points = m_Data.GetPoints();
    if(data)
    {
        Points::Pointer dataSetPoints = m_Data.GetPoints();
        char* data_p = const_cast<char*>(data);
        while (*data_p == '\n' || *data_p == ' ' || *data_p == '\t') data_p ++;
//        if(strcmp(attribute, "ascii") == 0){
//            float p[3] = { 0 };
//            token = strtok(data_p, delimiters);
//
//            while (token != nullptr) {
//                for(float & i : p) {
//                    i = mAtof(token);
//                    token = strtok(nullptr, delimiters);
//                }
//                dataSetPoints->AddPoint(p);
//            }
//        } else if(strcmp(attribute, "binary") == 0){
//            ReadBase64EncodedPoints(data_p, dataSetPoints);
//        }
        const char* type = elem->Attribute("type");
        attribute = elem->Attribute("format");
        if(attribute == nullptr || strcmp(attribute, "ascii") == 0){
            float p[3] = { 0 };
            token = strtok(data_p, delimiters);
            while (token != nullptr) {
                for(float & i : p) {
                    i = mAtof(token);
                    token = strtok(nullptr, delimiters);
                }
                dataSetPoints->AddPoint(p);
            }

        } else if(strcmp(attribute, "binary") == 0){
            if(!strncmp(type, "Float", 5)) {
                //  Float32
                if (!strncmp(type + 5, "32", 2)) {
                    ReadBase64EncodedPoints<float>(data_p, dataSetPoints);
                } else /*Float64*/{
                    ReadBase64EncodedPoints<double>(data_p, dataSetPoints);
                }
            }
        }
    }
    int vhs[16] = { 0 };
//    int CellNum = (x_dimension - 1) * (y_dimension - 1) * (z_dimension - 1);
    CellArray::Pointer volume = m_Data.GetVolumes();

    int xy_multi_dimensions = x_dimension * y_dimension;
    int xyz_multi_dimensions = x_dimension * y_dimension * z_dimension;
    for(int z = 0; z + xy_multi_dimensions < xyz_multi_dimensions + 0; z += xy_multi_dimensions)
    {
        for(int y = z; y + y_dimension < z + xy_multi_dimensions; y += y_dimension)
        {
            for(int x = y; x + 1 < y + y_dimension; x ++)
            {
                int xz = x + xy_multi_dimensions;

                vhs[0] = x ,  vhs[1] = x  + 1,  vhs[2] = x  + y_dimension + 1,  vhs[3] = x  + y_dimension;
                vhs[4] = xz,  vhs[5] = xz + 1,  vhs[6] = xz + y_dimension + 1,  vhs[7] = xz + y_dimension;
//                Hexahdrons->AddCell(vhs, 8);
                volume->AddCellIds(vhs, 8);
            }
        }
    }

    unsigned int pointNum =  Points->GetNumberOfPoints();
    // find Points' Scalar Data
    elem = FindTargetItem(root, "PointData");
    elem = elem->FirstChildElement("DataArray");
    //  use while loop to find point's multiple scala data.
    while(elem){
        data = elem->GetText();
        ArrayObject::Pointer  array;
        std::string scalarName = elem->Attribute("Name");
        const char* type = elem->Attribute("type");

//        int scalarComponents = mAtoi(elem->Attribute("NumberOfComponents"));
        if(data)
        {
            float range_max = FLT_MIN;
            float range_min = FLT_MAX;
            if(!strncmp(type, "Float", 5)){
                FloatArray::Pointer arr = FloatArray::New();
                float ps[3] = { 0 };
                token = strtok(const_cast<char*>(data), delimiters);
                while (token != nullptr) {
                    for(float & i : ps) {
                        i = mAtof(token);
//                        i /= 0.010064;
//                        i *= 0.12;
//                        i = std::abs(i);
                        if(i > range_max) range_max = i;
                        else if(i < range_min) range_min = i;
                        token = strtok(nullptr, delimiters);
                    }
                    arr->AddElement(ps);
                }
                array = arr;
            }
            if(array != nullptr){
                array->SetName(scalarName);

                m_Data.GetData()->AddScalar(IG_POINT, array, {range_min, range_max});
            }

        }


        elem = elem->NextSiblingElement("DataArray");
    }
    return true;
}
IGAME_NAMESPACE_END