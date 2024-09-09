//
// Created by Sumzeek on 9/9/2024.
//

#include "iGamePointPickedInteractor.h"

IGAME_NAMESPACE_BEGIN

void PointPickedInteractor::SetPointSet(PointSet* set, Model* model) {
    m_Points = set;
    m_Model = model;
}

void PointPickedInteractor::MousePressEvent(int posX, int posY,
                                            MouseButton mouseMode) {
    igm::vec2 pos = {float(posX), (float) posY};
    if (m_Points == nullptr || m_Model == nullptr) { return; }

    auto width = (float) m_Camera->GetViewPort().x;
    auto height = (float) m_Camera->GetViewPort().y;

    // NDC coordinate [-1,1]
    float x = 2.0f * pos.x / width - 1.0f;
    float y = 1.0f - (2.0f * pos.y / height);

    auto mvp = (m_Scene->CameraData().proj_view * m_Scene->ObjectData().model);
    auto mvp_invert = mvp.invert();

    // Clipping coordinate
    igm::vec4 point(x, y, 1.0, 1);
    igm::vec4 pointEnd(x, y, 0.001, 1);

    // World coordinate
    igm::vec4 tpoint = mvp_invert * point;
    igm::vec4 tpointEnd = mvp_invert * pointEnd;

    // 3D World coordinate
    igm::vec3 point1(tpoint / tpoint.w);
    igm::vec3 point2(tpointEnd / tpointEnd.w);

    igm::vec3 dir = (point1 - point2).normalized();

    PointPicker::Pointer picker = PointPicker::New();
    picker->SetPointSet(m_Points);
    igIndex id = picker->PickClosetPointWithLine(
            Vector3d(point1.x, point1.y, point1.z),
            Vector3d(dir.x, dir.y, dir.z));

    m_Model->GetPointPainter()->Clear();
    if (id != -1) {
        m_Model->GetPointPainter()->DrawPoint(m_Points->GetPoint(id));
    }
}

void PointPickedInteractor::MouseMoveEvent(int eventX, int eventY) {}

void PointPickedInteractor::MouseReleaseEvent(int eventX, int eventY) {}

void PointPickedInteractor::WheelEvent(double delta) {}

IGAME_NAMESPACE_END