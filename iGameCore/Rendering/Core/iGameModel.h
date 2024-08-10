#ifndef OPENIGAME_MODEL_H
#define OPENIGAME_MODEL_H

#include "iGameObject.h"
#include "iGameDataObject.h"
#include "iGamePoints.h"
#include "iGamePointPainter.h"
#include "iGameLinePainter.h"

IGAME_NAMESPACE_BEGIN
class Scene;

class Model : public Object {
public:
    I_OBJECT(Model);
    static Pointer New() { return new Model; }

    void Draw(Scene*);

    PointPainter* GetPointPainter() {
        return m_PickedPointPainter.get();
    }
    LinePainter* GetLinePainter() { return m_PickedLinePainter.get(); }

    void Show() { 
        m_DataObject->SetVisibility(true);
    }
    void Hide() { 
        m_DataObject->SetVisibility(false);
    }

    void ShowBoundingBox() { 
        m_BoundingBoxPainter->SetVisibility(true);
        auto& bbox = m_DataObject->GetBoundingBox();

        double length = bbox.max[0] - bbox.min[0];
        double width = bbox.max[1] - bbox.min[1];
        double height = bbox.max[2] - bbox.min[2];

        Vector3d p1 = bbox.min;
        Vector3d p2(bbox.min[0] + length, bbox.min[1], bbox.min[2]);
        Vector3d p3(bbox.min[0] + length, bbox.min[1] + width, bbox.min[2]);
        Vector3d p4(bbox.min[0], bbox.min[1] + width, bbox.min[2]);
        Vector3d p5(bbox.min[0], bbox.min[1], bbox.min[2] + height);
        Vector3d p6(bbox.max[0], bbox.max[1] - width, bbox.max[2]);
        Vector3d p7 = bbox.max;
        Vector3d p8(bbox.max[0] - length, bbox.max[1], bbox.max[2]);

        m_BoundingBoxPainter->Clear();

        m_BoundingBoxPainter->DrawLine(p1, p2);
        m_BoundingBoxPainter->DrawLine(p2, p3);
        m_BoundingBoxPainter->DrawLine(p3, p4);
        m_BoundingBoxPainter->DrawLine(p4, p1);

        m_BoundingBoxPainter->DrawLine(p1, p5);
        m_BoundingBoxPainter->DrawLine(p2, p6);
        m_BoundingBoxPainter->DrawLine(p3, p7);
        m_BoundingBoxPainter->DrawLine(p4, p8);

        m_BoundingBoxPainter->DrawLine(p5, p6);
        m_BoundingBoxPainter->DrawLine(p6, p7);
        m_BoundingBoxPainter->DrawLine(p7, p8);
        m_BoundingBoxPainter->DrawLine(p8, p5);
    }
    void HideBoundingBox() { 
        m_BoundingBoxPainter->SetVisibility(false);
    }

    void Update();

protected:
    Model();
    ~Model() override = default;

    DataObject::Pointer m_DataObject{};
    Scene* m_Scene{nullptr};
    PointPainter::Pointer m_PickedPointPainter{};
    LinePainter::Pointer m_PickedLinePainter{};
    LinePainter::Pointer m_BoundingBoxPainter{};

    friend class Scene;
};

IGAME_NAMESPACE_END
#endif // OPENIGAME_SCENCE_H
