#ifndef OPENIGAME_PAINTER_H
#define OPENIGAME_PAINTER_H

#include "iGameObject.h"
#include "iGameDataObject.h"
#include "iGamePoints.h"

IGAME_NAMESPACE_BEGIN
class Scene;

class Painter : public Object {
public:
    I_OBJECT(Painter);

    virtual void Clear() = 0;

    virtual void Draw(Scene*) = 0;

    virtual void SetVisibility(bool f) { m_Visibility = f; }

protected:
    Painter();
    ~Painter() override = default;

    Vector3f ColorMap(Color c);

    bool m_Visibility{true};
};

IGAME_NAMESPACE_END
#endif 
