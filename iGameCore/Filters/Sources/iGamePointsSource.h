//
// Created by m_ky on 2024/8/7.
//

/**
 * @class   iGamePointsSource
 * @brief   iGamePointsSource's brief
 */
#pragma once

#include "iGameFilter.h"
#include "iGameUnstructuredMesh.h"


IGAME_NAMESPACE_BEGIN
class PointSource : public Filter{
    I_OBJECT(PointSource)

public:
    static Pointer New(){return new PointSource;}

    bool Execute() override {
        return Filter::Execute();
    }



protected:
    PointSource()
    {
        SetNumberOfInputs(1);
        SetNumberOfOutputs(1);
        m_OutPut_PointSet = UnstructuredMesh::New();
        m_OutPut_PointSet->AddPoint(Point(0.f, 0.f, 0.f));
        m_OutPut_PointSet->AddPoint(Point(1.f, 1.0f, 1.f));
        igIndex cell[1] = {0};
        m_OutPut_PointSet->AddCell(cell, 1, IG_VERTEX);
        cell[0] = 1;
        m_OutPut_PointSet->AddCell(cell, 1, IG_VERTEX);
        SetOutput(m_OutPut_PointSet);
    }
    ~PointSource() override = default;

protected:
    UnstructuredMesh::Pointer m_OutPut_PointSet;
};
IGAME_NAMESPACE_END