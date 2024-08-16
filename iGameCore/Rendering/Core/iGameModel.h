#ifndef OPENIGAME_MODEL_H
#define OPENIGAME_MODEL_H

#include <utility>

#include "iGameObject.h"
#include "iGameDataObject.h"
#include "iGamePoints.h"
#include "iGamePointPainter.h"
#include "iGameLinePainter.h"
#include "iGameFilter.h"

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
    Filter* GetModelFilter(){ return m_Filter; }
    void SetModelFilter(Filter* _filter){ m_Filter = _filter; }
    void Show();
    void Hide();
    void SetBoundingBoxSwitch(bool action);
    void SetPickedItemSwitch(bool action);
    void SetViewPointsSwitch(bool action);
    void SetViewWireframeSwitch(bool action);
    void SetViewFillSwitch(bool action);

    void Update();

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

    Filter* m_Filter{nullptr};
    DataObject::Pointer m_DataObject{};
    Scene* m_Scene{nullptr};
    PointPainter::Pointer m_PickedPointPainter{};
    LinePainter::Pointer m_PickedLinePainter{};
    LinePainter::Pointer m_BBoxPainter{};
    unsigned long long m_Switch{0ull};

    friend class Scene;
};

IGAME_NAMESPACE_END
#endif // OPENIGAME_SCENCE_H
