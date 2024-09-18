//#ifndef OPENIGAME_FACE_PAINTER_H
//#define OPENIGAME_FACE_PAINTER_H
//
//#include "iGamePainterX.h"
//
//IGAME_NAMESPACE_BEGIN
//class Scene;
//
//class FacePainter : public PainterX {
//public:
//    I_OBJECT(FacePainter);
//    static Pointer New() { return new FacePainter; }
//
//    void DrawTriangle(const Point& p1, const Point& p2, const Point& p3,
//                      Color c = Color::None);
//
//    void Clear() override;
//
//    void Draw(Scene*) override;
//
//protected:
//    FacePainter();
//    ~FacePainter() override = default;
//
//    Points::Pointer m_Points{};
//    FloatArray::Pointer m_Colors{};
//
//    GLVertexArray VAO;
//    GLBuffer pVBO, cVBO;
//};
//
//IGAME_NAMESPACE_END
//#endif
