//
// Created by Sumzeek on 9/9/2024.
//

#include "iGameFacesSelection.h"

IGAME_NAMESPACE_BEGIN

void FacesSelection::SetModel(Model* model) {
    if (model) {
        m_Model = model;
        m_Mesh = DynamicCast<SurfaceMesh>(model->GetDataObject());
    }
}

void FacesSelection::SelectElement(const std::vector<igm::vec4>& planes) {
    //m_Model->GetFacePainter()->DrawTriangle(Point(0, 0, 0), Point(1, 0, 0),
    //                                        Point(0, 1, 0));
    std::vector<bool> pvis(m_Mesh->GetNumberOfPoints(), false);
    m_Model->GetFacePainter()->Clear();
    for (int i = 0; i < m_Mesh->GetNumberOfPoints(); i++) {
        auto& p = m_Mesh->GetPoint(i);
        if (IsPointInFrustum(igm::vec3(p[0], p[1], p[2]), planes)) {
            pvis[i] = true;
        }
    }

    igIndex face[16]{};
    for (int i = 0; i < m_Mesh->GetNumberOfFaces(); i++) {
        int size = m_Mesh->GetFacePointIds(i, face);
        bool flag = false;
        for (int j = 0; j < size; j++) {
            if (pvis[face[j]]) {
                flag = true;
                break;
            }
        }
        if (flag) {
            for (int j = 2; j < size; j++) {
                m_Model->GetFacePainter()->DrawTriangle(
                        m_Mesh->GetPoint(face[0]),
                        m_Mesh->GetPoint(face[j - 1]),
                        m_Mesh->GetPoint(face[j]));

                //std::cout << "--------------\n";
                //std::cout << m_Mesh->GetPoint(face[0]) << std::endl;
                //std::cout << m_Mesh->GetPoint(face[j - 1]) << std::endl;
                //std::cout << m_Mesh->GetPoint(face[j]) << std::endl;
            }
        }
    }
}

bool FacesSelection::IsPointInFrustum(const igm::vec3& p,
                                      const std::vector<igm::vec4>& planes) {

    for (int i = 0; i < 6; i++) {
        auto& plane = planes[i];
        float dist = p.x * plane.x + p.y * plane.y + p.z * plane.z + plane.w;
        if (dist < 0) { return false; }
    }

    return true;
}

IGAME_NAMESPACE_END