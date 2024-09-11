//
// Created by Sumzeek on 9/9/2024.
//

#include "iGamePointsSelection.h"

IGAME_NAMESPACE_BEGIN

void PointsSelection::SetPointSet(PointSet* set, Model* model) {
    m_Points = set;
    m_Model = model;
}

void PointsSelection::SelectElement(const std::vector<igm::vec4>& planes) {
    m_Model->GetPointPainter()->Clear();
    for (int i = 0; i < m_Points->GetNumberOfPoints(); i++) {
        auto& p = m_Points->GetPoint(i);

        if (IsPointInFrustum(igm::vec3(p[0], p[1], p[2]), planes)) {
            m_Model->GetPointPainter()->DrawPoint(p);
        }
    }
}

bool PointsSelection::IsPointInFrustum(const igm::vec3& p,
                                       const std::vector<igm::vec4>& planes) {

    for (int i = 0; i < 6; i++) {
        auto& plane = planes[i];
        float dist = p.x * plane.x + p.y * plane.y + p.z * plane.z + plane.w;
        if (dist < 0) { return false; }
    }

    return true;
}

IGAME_NAMESPACE_END