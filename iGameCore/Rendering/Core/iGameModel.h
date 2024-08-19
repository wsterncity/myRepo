#ifndef OPENIGAME_MODEL_H
#define OPENIGAME_MODEL_H

#include "iGameObject.h"
#include "iGameDataObject.h"
#include "iGamePoints.h"
#include "iGamePointPainter.h"
#include "iGameLinePainter.h"
#include "iGameFacePainter.h"

IGAME_NAMESPACE_BEGIN
class Scene;
class Model : public Object {
public:
    I_OBJECT(Model);
    static Pointer New() { return new Model; }

    void Draw(Scene*);

    DataObject::Pointer GetDataObject() { return m_DataObject; }
    bool GetVisibility() { return m_DataObject->GetVisibility(); }
    PointPainter* GetPointPainter() { return m_PickedPointPainter.get();}
    LinePainter* GetLinePainter() { return m_PickedLinePainter.get(); }
    FacePainter* GetFacePainter() { return m_PickedFacePainter.get(); }

    void Show();
    void Hide();
    void SetBoundingBoxSwitch(bool action);
    void SetPickedItemSwitch(bool action);
    void SetViewPointsSwitch(bool action);
    void SetViewWireframeSwitch(bool action);
    void SetViewFillSwitch(bool action);

    void Update();

    void ViewCloudPicture(int index, int dimension = -1) {
        m_DataObject->ViewCloudPicture(m_Scene, index, dimension);
    }

protected:
    Model();
    ~Model() override = default;

    enum ViewSwitch{
        BoundingBox = 0,
        PickedItem
    };

    void SwitchOn(ViewSwitch type) { m_Switch |= (1ull << type); }
    void SwitchOff(ViewSwitch type) { m_Switch &= ~(1ull << type); }
    bool GetSwitch(ViewSwitch type) { return m_Switch & (1ull << type); }

    DataObject::Pointer m_DataObject{};
    Scene* m_Scene{nullptr};
    PointPainter::Pointer m_PickedPointPainter{};
    LinePainter::Pointer m_PickedLinePainter{};
    FacePainter::Pointer m_PickedFacePainter{};
    LinePainter::Pointer m_BBoxPainter{};
    unsigned long long m_Switch{0ull};

    friend class Scene;
};

IGAME_NAMESPACE_END
#endif // OPENIGAME_SCENCE_H
