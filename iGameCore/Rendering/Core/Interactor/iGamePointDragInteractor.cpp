//
// Created by Sumzeek on 9/9/2024.
//

#include "iGamePointDragInteractor.h"

IGAME_NAMESPACE_BEGIN

void PointDragInteractor::SetPointSet(PointSet::Pointer ps,
                                      Model::Pointer model) {
    m_PointSet = ps;
    m_Model = model;
    pointPicker = PointPicker::New();
    pointPicker->SetPointSet(m_PointSet);
}

void PointDragInteractor::MouseReleaseEvent(int _eventX, int _eventY) {
    m_MouseMode = NoButton;
    Interactor::MouseReleaseEvent(_eventX, _eventY);
}
void PointDragInteractor::MouseMoveEvent(int posX, int posY) {
    igm::vec2 pos = {float(posX), (float) posY};

    // Screen coordinates
    auto width = (float) m_Camera->GetViewPort().x;
    auto height = (float) m_Camera->GetViewPort().y;

    // Convert screen coordinates to NDC coordinates [-1,1]
    float x = 2.0f * pos.x / width - 1.0f;
    float y = 1.0f - (2.0f * pos.y / height);

    auto mvp = (m_Scene->CameraData().proj_view * m_Scene->ObjectData().model);
    if (mvp.determinant() == 0) return;
    auto mvp_invert = mvp.invert();
    if (m_MouseMode == NoButton) {
        // Convert NDC coordinates to clipping coordinates
        igm::vec4 point(x, y, 1, 1);
        igm::vec4 pointEnd(x, y, 0.001, 1);

        // Clip coordinates to world coordinates
        igm::vec4 tpoint = mvp_invert * point;
        igm::vec4 tpointEnd = mvp_invert * pointEnd;

        // 3 dimensional world coordinates
        igm::vec3 point1(tpoint / tpoint.w);
        igm::vec3 point2(tpointEnd / tpointEnd.w);
        igm::vec3 dir = (point1 - point2).normalized();
        Selected_Point_Index = pointPicker->PickClosetPointWithLine(
                Vector3d(point1.x, point1.y, point1.z),
                Vector3d(dir.x, dir.y, dir.z));

        if (~Selected_Point_Index) {
            auto tp = m_PointSet->GetPoint(Selected_Point_Index);
            igm::vec4 p{tp[0], tp[1], tp[2], 1.f};
            p = mvp * p;
            Selected_NDC_Z = p.z / p.w;
        }
        return;
    } else if (m_MouseMode == LeftButton) {
        if (Selected_Point_Index == -1) {
//            BasicInteractor::MouseMoveEvent(posX, posY);
            return;
        }

        igm::vec4 Point_NDC{x, y, Selected_NDC_Z, 1.f};
        igm::vec4 newPoint_WorldCoord = mvp_invert * Point_NDC;
        newPoint_WorldCoord /= newPoint_WorldCoord.w;
        m_PointSet->SetPoint(Selected_Point_Index,
                             Vector3f{newPoint_WorldCoord.x,
                                      newPoint_WorldCoord.y,
                                      newPoint_WorldCoord.z});
        m_PointSet->Modified();
    } /*else {
        BasicInteractor::MouseMoveEvent(posX, posY);
    }*/
}

void PointDragInteractor::MousePressEvent(int eventX, int eventY, MouseButton _mouseMode) {
    m_MouseMode = _mouseMode;
}

IGAME_NAMESPACE_END