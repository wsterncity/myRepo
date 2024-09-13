//
// Created by Sumzeek on 9/9/2024.
//

#include "iGameSelectionStyle.h"
#include "iGameInteractor.h"
#include "iGamePointPicker.h"

IGAME_NAMESPACE_BEGIN
void SelectionStyle::SetSelectedType(SelectedType type) { m_Type = type; }

SelectionStyle::SelectedType SelectionStyle::GetSelectedType() const {
    return m_Type;
}

void SelectionStyle::Initialize(Interactor* a, Selection* s) {
    BasicStyle::Initialize(a);
    if (s) { 
        m_Selection = s; 
        m_Points = s->GetPoints();
        m_Cells = s->GetCells();
        m_Model = s->GetModel();
    }
}

void SelectionStyle::MousePressEvent(IEvent _event) { 
	BasicStyle::MousePressEvent(_event);
}

void SelectionStyle::LeftButtonMouseMove() {}

void SelectionStyle::RightButtonMouseMove() { BasicStyle::ModelRotation(); }

void SelectionStyle::MiddleButtonMouseMove() { BasicStyle::ViewTranslation(); }

bool SelectionStyle::IsIntersectTriangle(igm::vec3 orig, igm::vec3 end,
                                               igm::vec3 v0, igm::vec3 v1,
                                               igm::vec3 v2,
                                               igm::vec3& intersection) {
    float t, u, v;
    igm::vec3 dir = end - orig;
    igm::vec3 E1 = v1 - v0;
    igm::vec3 E2 = v2 - v0;
    // cross product
    igm::vec3 P(dir.y * E2.z - E2.y * dir.z, E2.x * dir.z - E2.z * dir.x,
                dir.x * E2.y - dir.y * E2.x);
    // dot product
    float det = E1.x * P.x + E1.y * P.y + E1.z * P.z;
    igm::vec3 T;
    if (det > 0) {
        T = orig - v0;
    } else {
        T = v0 - orig;
        det = -det;
    }
    if (det < 0) { return false; }

    // Calculate u and make sure u <= 1
    u = T.x * P.x + T.y * P.y + T.z * P.z;
    if (u < 0.0f || u > det) { return false; }

    igm::vec3 Q(T.y * E1.z - E1.y * T.z, E1.x * T.z - E1.z * T.x,
                T.x * E1.y - T.y * E1.x);

    // Calculate v and make sure u + v <= 1
    v = dir.x * Q.x + dir.y * Q.y + dir.z * Q.z;
    if (v < 0.0f || u + v > det) { return false; }

    // Calculate t, scale parameters, ray intersects triangle
    float fInvDet = 1.0f / det;
    u *= fInvDet;
    v *= fInvDet;
    intersection = E1 * u + E2 * v + v0;
    return true;
}


double SelectionStyle::DistancePointToPlane(igm::vec3 point, igm::vec3 p1,
                                                  igm::vec3 p2, igm::vec3 p3) {
    // Calculate two vectors on the plane
    igm::vec3 v1 = {p2.x - p1.x, p2.y - p1.y, p2.z - p1.z};
    igm::vec3 v2 = {p3.x - p1.x, p3.y - p1.y, p3.z - p1.z};

    // Calculate the normal vector of the plane
    igm::vec3 n = v1.cross(v2);

    // Calculate the d constant in the plane equation
    double d = -n.dot(p1);

    // Calculate the distance from the point to the plane
    double numerator =
            std::abs(n.x * point.x + n.y * point.y + n.z * point.z + d);
    double denominator = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);

    return numerator / denominator;
}

igm::vec4 SelectionStyle::GetPlane(const igm::vec3& p,
                                     const igm::vec3& normal) {
    igm::vec3 n = normal.normalized();
    return igm::vec4(n, -n.dot(p));
}

igm::vec3 SelectionStyle::GetNearWorldCoord(const igm::vec2& screenCoord,
                                              const igm::mat4& invertedMvp) {
    igm::vec2 NDC(2.0f * screenCoord.x / m_Interactor->GetWidth() - 1.0f,
                  1.0f - (2.0f * screenCoord.y / m_Interactor->GetHeight()));

    // Clipping coordinate
    igm::vec4 clippingCoord(NDC, 1, 1.0);

    // World coordinate
    igm::vec4 worldCoord = invertedMvp * clippingCoord;
    return igm::vec3(worldCoord / worldCoord.w);
}


igm::vec3 SelectionStyle::GetFarWorldCoord(const igm::vec2& screenCoord,
                                             const igm::mat4& invertedMvp) {
    igm::vec2 NDC(2.0f * screenCoord.x / m_Interactor->GetWidth() - 1.0f,
                  1.0f - (2.0f * screenCoord.y / m_Interactor->GetHeight()));

    // Clipping coordinate
    igm::vec4 clippingCoord(NDC, 0.001, 1.0);

    // World coordinate
    igm::vec4 worldCoord = invertedMvp * clippingCoord;
    return igm::vec3(worldCoord / worldCoord.w);
}
IGAME_NAMESPACE_END


