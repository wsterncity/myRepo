#ifndef OPENIGAME_MODEL_H
#define OPENIGAME_MODEL_H

#include "iGameObject.h"
#include "iGameDataObject.h"
#include "iGamePoints.h"
#include "iGamePointPainter.h"

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

protected:
    Model();
    ~Model() override = default;

    DataObject::Pointer m_DataObject{};

    PointPainter::Pointer m_PickedPointPainter{};

    friend class Scene;
};

IGAME_NAMESPACE_END
#endif // OPENIGAME_SCENCE_H
