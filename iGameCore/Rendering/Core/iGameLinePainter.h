#ifndef OPENIGAME_LINE_PAINTER_H
#define OPENIGAME_LINE_PAINTER_H

#include "iGamePainter.h"

IGAME_NAMESPACE_BEGIN
class Scene;

class LinePainter : public Painter {
public:
    I_OBJECT(LinePainter);
    static Pointer New() { return new LinePainter; }

    void SetLineWidth(int width) {
        m_LineWidth = width;
        this->Modified();
    }

    void DrawLine(const Point& p1, const Point& p2, Color c = Color::None);

    void Clear() override;

    void Draw(Scene*) override;

protected:
    LinePainter();
    ~LinePainter() override = default;

    Points::Pointer m_Points{};
    FloatArray::Pointer m_Colors{};

    GLVertexArray VAO;
    GLBuffer pVBO, cVBO;
    int m_LineWidth{2};
};

IGAME_NAMESPACE_END
#endif 
