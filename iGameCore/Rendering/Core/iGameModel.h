#ifndef OPENIGAME_MODEL_H
#define OPENIGAME_MODEL_H

#include <utility>

#include "iGameDataObject.h"
#include "iGameFilter.h"
#include "iGameObject.h"
#include "iGamePainter.h"
#include "iGamePoints.h"

IGAME_NAMESPACE_BEGIN
class Scene;
class Filter;
class Model : public Object {
public:
    I_OBJECT(Model);
    static Pointer New() { return new Model; }

    void Draw(Scene*);

    DataObject::Pointer GetDataObject() { return m_DataObject; }
    bool GetVisibility() { return m_DataObject->GetVisibility(); }
   Filter* GetModelFilter();
   Painter* GetPainter() { return m_Painter; }
    void DeleteModelFilter();
    void SetModelFilter(SmartPointer<Filter> _filter);
    void SetDataObject(DataObject::Pointer dataObject){m_DataObject = dataObject;}
    void Modified() { m_DataObject->Modified(); }

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
    void SetFilePath(std::string filePath) { m_FilePath = filePath; }
    std::string GetFilePath() { return this->m_FilePath; }

    Selection* GetSelection();
    void RequestPointSelection(Points* p, Selection* s);
    void RequestDragPoint(Points* p, Selection* s);

protected:
    Model();
    ~Model() override = default;

    enum ViewSwitch { BoundingBox = 0, PickedItem };

    void SwitchOn(ViewSwitch type) { m_Switch |= (1ull << type); }
    void SwitchOff(ViewSwitch type) { m_Switch &= ~(1ull << type); }
    bool GetSwitch(ViewSwitch type) { return m_Switch & (1ull << type); }

    Selection::Pointer m_Selection{};
    SmartPointer<Filter> m_Filter{};
    DataObject::Pointer m_DataObject{};
    std::string m_FilePath;
    Scene* m_Scene{nullptr};
    Painter::Pointer m_Painter{};
    IGuint m_BboxHandle = 0;
    unsigned long long m_Switch{0ull};

    friend class Scene;
};

IGAME_NAMESPACE_END
#endif // OPENIGAME_SCENCE_H
