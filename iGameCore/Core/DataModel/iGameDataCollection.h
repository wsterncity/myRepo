//
// Created by m_ky on 2024/7/14.
//

/**
 * @class   iGameDataCollection
 * @brief   iGameDataCollection's brief
 */
#pragma once

#include "iGameDataObject.h"
#include "iGameCellArray.h"

IGAME_NAMESPACE_BEGIN
class DataCollection {
public:
    Points::Pointer        Points{};
    //AttributeData::Pointer PointData{};
    //AttributeData::Pointer CellData{};
    AttributeData::Pointer Data{};
    StreamingData::Pointer Time_Data{};

    CellArray::Pointer Lines{};

    // 表面结构
    CellArray::Pointer Faces{};

    // 体结构
    CellArray::Pointer Volumes{};

    IGenum Type{};

    Points::Pointer GetPoints()
    {
        if (Points == nullptr)
        {
            Points = Points::New();
        }
        return Points;
    }
    AttributeData::Pointer GetData()
    {
        if (Data == nullptr)
        {
            Data = AttributeData::New();
        }
        return Data;
    }
    StreamingData::Pointer GetTimeData()
    {
        if (Time_Data == nullptr)
        {
            Time_Data = StreamingData::New();
        }
        return Time_Data;
    }
    //AttributeData::Pointer GetPointData()
    //{
    //	if (PointData == nullptr)
    //	{
    //		PointData = AttributeData::New();
    //	}
    //	return PointData;
    //}
    //AttributeData::Pointer GetCellData()
    //{
    //	if (CellData == nullptr)
    //	{
    //		CellData = AttributeData::New();
    //	}
    //	return CellData;
    //}
    CellArray::Pointer GetLines()
    {
        if (Lines == nullptr)
        {
            Lines = CellArray::New();
        }
        return Lines;
    }
    CellArray::Pointer GetFaces()
    {
        if (Faces == nullptr)
        {
            Faces = CellArray::New();
        }
        return Faces;
    }
    CellArray::Pointer GetVolumes()
    {
        if (Volumes == nullptr)
        {
            Volumes = CellArray::New();
        }
        return Volumes;
    }


    unsigned int GetNumberOfPoints() { return Points ? Points->GetNumberOfPoints() : 0; }
    unsigned int GetNumberOfLines() { return Lines ? Lines->GetNumberOfCells() : 0; }
    unsigned int GetNumberOfFaces() { return Faces ? Faces->GetNumberOfCells() : 0; }
    unsigned int GetNumberOfVolumes() { return Volumes ? Volumes->GetNumberOfCells() : 0; }
    unsigned int GetNumberOfCells() { return GetNumberOfLines() + GetNumberOfFaces() + GetNumberOfVolumes(); }

    void AddPoint(Point p) { GetPoints()->AddPoint(p); }
    void AddLine(igIndex* vhs, igIndex size = 2) { GetLines()->InsertNextCell(vhs, size);}
    void AddFace(igIndex* vhs, igIndex size) { GetFaces()->InsertNextCell(vhs, size);}
    void AddVolume(igIndex* vhs, igIndex size) { GetVolumes()->InsertNextCell(vhs, size);}
};

IGAME_NAMESPACE_END