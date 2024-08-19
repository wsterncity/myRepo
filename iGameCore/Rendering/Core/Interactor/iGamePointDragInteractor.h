//
// Created by m_ky on 2024/8/8.
//

/**
 * @class   iGamePointDragInteractor
 * @brief   iGamePointDragInteractor's brief
 */
#pragma once

#include "iGameInteractor.h"

IGAME_NAMESPACE_BEGIN
class PointDragInteractor : public Interactor {
public:
    I_OBJECT(PointDragInteractor)

    static Pointer New() { return new PointDragInteractor; }

    virtual void SetPointSet(PointSet::Pointer ps,
                             Model::Pointer model = nullptr) {
        m_PointSet = ps;
        m_Model = model;
        pointPicker = PointPicker::New();
        pointPicker->SetPointSet(m_PointSet);
    }

    //    void MousePressEvent(int posX, int posY, MouseButton _mouseMode) override {
    //        m_MouseMode = _mouseMode;
    //    }
    //
    void MouseReleaseEvent(int _eventX, int _eventY) override {
        m_MouseMode = NoButton;
        Interactor::MouseReleaseEvent(_eventX, _eventY);
    }

    void MouseMoveEvent(int posX, int posY) override {
        igm::vec2 pos = {float(posX), (float) posY};
        m_Width = m_Camera->GetViewPort().x;
        m_Height = m_Camera->GetViewPort().y;
        m_DevicePixelRatio = m_Camera->GetDevicePixelRatio();

        // ��Ļ����
        float width = (float) m_Width / m_DevicePixelRatio;
        float height = (float) m_Height / m_DevicePixelRatio;

        // ����Ļ����תΪNDC����[-1,1]
        float x = 2.0f * pos.x / width - 1.0f;
        float y = 1.0f - (2.0f * pos.y / height);

        auto mvp =
                (m_Scene->CameraData().proj_view * m_Scene->ObjectData().model);
        if (mvp.determinant() == 0) return;
        auto mvp_invert = mvp.invert();
        if (m_MouseMode == NoButton) {
            // NDC����תΪ�ü�����
            igm::vec4 point(x, y, 0, 1);      // ��ƽ���
            igm::vec4 pointEnd(x, y, 1.0, 1); // Զƽ���

            // �ü�����תΪ��������
            igm::vec4 tpoint = mvp_invert * point;
            igm::vec4 tpointEnd = mvp_invert * pointEnd;

            // 3ά����������
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
                Interactor::MouseMoveEvent(posX, posY);
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
        } else {
            Interactor::MouseMoveEvent(posX, posY);
        }
    }

protected:
    PointDragInteractor() = default;

    PointPicker::Pointer pointPicker{};
    PointSet::Pointer m_PointSet{};
    Model::Pointer m_Model{};
    igIndex Selected_Point_Index{-1};

    float Selected_NDC_Z{0};
    int m_Width{}, m_Height{};
    int m_DevicePixelRatio{};
};

IGAME_NAMESPACE_END