//
// Created by m_ky on 2024/8/7.
//

/**
 * @class   iGameLineTypePointsSource
 * @brief   iGameLineTypePointsSource's brief
 */

#include "iGameLineTypePointsSource.h"
IGAME_NAMESPACE_BEGIN

bool LineTypePointsSource::Execute() {
    if(this->GetInput(0) != nullptr){
        UnstructuredMesh::Pointer ps = DynamicCast<UnstructuredMesh>(this->GetInput(0));
        if(ps != nullptr) m_OutPut_PointSet = ps;
    }


    return PointSource::Execute();
}

void LineTypePointsSource::SetPoint_0(const float point0[3]) {
    m_Point_0[0] = point0[0];
    m_Point_0[1] = point0[1];
    m_Point_0[2] = point0[2];
}

void LineTypePointsSource::SetPoint_1(const float point1[3]) {
    m_Point_1[0] = point1[0];
    m_Point_1[1] = point1[1];
    m_Point_1[2] = point1[2];
}


IGAME_NAMESPACE_END