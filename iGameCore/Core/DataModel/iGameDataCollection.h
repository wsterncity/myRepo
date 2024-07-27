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

    PropertySet::Pointer Data{};

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
    PropertySet::Pointer GetData()
    {
        if (Data == nullptr)
        {
            Data = PropertySet::New();
        }
        return Data;
    }
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
    void AddLine(igIndex* vhs, igIndex size = 2) { GetLines()->AddCellIds(vhs, size);}
    void AddFace(igIndex* vhs, igIndex size) { GetFaces()->AddCellIds(vhs, size);}
    void AddVolume(igIndex* vhs, igIndex size) { GetVolumes()->AddCellIds(vhs, size);}
};

IGAME_NAMESPACE_END