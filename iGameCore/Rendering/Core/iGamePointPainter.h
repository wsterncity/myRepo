#ifndef OPENIGAME_POINT_PAINTER_H
#define OPENIGAME_POINT_PAINTER_H

#include "iGamePainter.h"

IGAME_NAMESPACE_BEGIN
class Scene;

class PointPainter : public Painter {
public:
    I_OBJECT(PointPainter);
    static Pointer New() { return new PointPainter; }

    void SetPointSize(int size);

    void DrawPoint(const Point& p, Color c = None);

    void Clear() override;

    void Draw(Scene*) override;

protected:
    PointPainter();
    ~PointPainter() override = default;

    Points::Pointer m_Points{};
    FloatArray::Pointer m_Colors{};

    GLVertexArray VAO;
    GLBuffer pVBO, cVBO;
    int m_PointSize{4};
};

IGAME_NAMESPACE_END
#endif 
