#ifndef OPENIGAME_POINT_PICKED_INTERACTOR_H
#define OPENIGAME_POINT_PICKED_INTERACTOR_H

#include "iGameInteractor.h"
#include "iGamePointSet.h"
#include "iGamePointPicker.h"

IGAME_NAMESPACE_BEGIN
class PointPickedInteractor : public Interactor {
public:
    I_OBJECT(PointPickedInteractor);
    static Pointer New() { return new PointPickedInteractor; }

    void SetPointSet(PointSet::Pointer set) {
        m_PointSet = set;
    }

    void MousePressEvent(int posX, int posY,
        MouseButton mouseMode) override
    {
        igm::vec2 pos = {float(posX), (float) posY};
        if (m_PointSet == nullptr)
        {
            return;
        }
        
        m_Width = m_Camera->GetViewPort().x;
        m_Height = m_Camera->GetViewPort().y;
        m_DevicePixelRatio = m_Camera->GetDevicePixelRatio();

        // 屏幕坐标
        float width = (float) m_Width / m_DevicePixelRatio;
        float height = (float) m_Height / m_DevicePixelRatio;

        // 将屏幕坐标转为NDC坐标[-1,1]
        float x = 2.0f * pos.x / width - 1.0f;
        float y = 1.0f - (2.0f * pos.y / height);

        auto mvp = (m_Scene->MVP().viewporj * m_Scene->MVP().model);
        auto mvp_invert = mvp.invert();

        // NDC坐标转为裁剪坐标
        igm::vec4 point(x, y, 0, 1);      // 近平面点
        igm::vec4 pointEnd(x, y, 1.0, 1); // 远平面点

        // 裁剪坐标转为世界坐标
        igm::vec4 tpoint = mvp_invert * point;  
        igm::vec4 tpointEnd = mvp_invert * pointEnd;

        // 3维的世界坐标
        igm::vec3 point1(tpoint / tpoint.w);
        igm::vec3 point2(tpointEnd / tpointEnd.w);

        igm::vec3 dir = (point1 - point2).normalized();

        PointPicker::Pointer picker = PointPicker::New();
        picker->SetPointSet(m_PointSet);
        igIndex id = picker->PickClosetPointWithLine(
                Vector3d(point1.x, point1.y, point1.z),
                Vector3d(dir.x, dir.y, dir.z));

        if (id != -1) {
            PointSet::Pointer p = PointSet::New();
            p->AddPoint(m_PointSet->GetPoint(id));
            m_Scene->AddDataObject(p);
        }
    }
    void MouseMoveEvent(int posX, int posY) override
    {

    }
    void MouseReleaseEvent(int posX, int posY) override
    {

    }
    void WheelEvent(double delta) override
    {

    }

protected:
    PointPickedInteractor() {}
    ~PointPickedInteractor() override = default;

    PointSet::Pointer m_PointSet{};

    int m_Width{}, m_Height{};
    int m_DevicePixelRatio{};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H