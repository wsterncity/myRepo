////
//// Created by Sumzeek on 9/12/2024.
////
//
//#pragma once
//
//#include "OpenGL/GLBuffer.h"
//#include "OpenGL/GLVertexArray.h"
//#include "iGameBrush.h"
//#include "iGameHandlePool.h"
//#include "iGameObject.h"
//#include "iGamePen.h"
//#include "iGamePoints.h"
//
//IGAME_NAMESPACE_BEGIN
//class Scene;
//
//class Painter : public Object {
//public:
//    I_OBJECT(Painter);
//    static Pointer New() { return new Painter; }
//
//public:
//    void ShowAll();
//    void HideAll();
//    void Show(IGuint handle);
//    void Hide(IGuint handle);
//
//    void SetPen(const Pen::Pointer& pen);
//    void SetPen(const Color& color);
//    void SetPen(const PenStyle& style);
//    void SetPen(int width);
//
//    void SetBrush(const Brush::Pointer& brush);
//    void SetBrush(const Color& color);
//    void SetBrush(const BrushStyle& style);
//
//    IGuint DrawPoint(const Point& point);
//    IGuint DrawLine(const Point& p1, const Point& p2);
//    IGuint DrawTriangle(const Point& p1, const Point& p2, const Point& p3);
//
//    void Draw(Scene*);
//    void Clear();
//
//protected:
//    void PenWidthUpdate();
//    Vector3f ColorMap(Color color);
//    void ExpandVBO(GLBuffer& vbo, size_t oldSize, size_t newSize);
//    void ExpandEBO(GLBuffer& ebo, size_t oldSize, size_t newSize);
//
//protected:
//    using IndexMapper = std::unordered_map<int, std::vector<iguIndex>>;
//    using EBOMapper = std::unordered_map<int, GLBuffer>;
//    using EBOSizeMapper = std::unordered_map<int, IGsize>;
//
//protected:
//    Painter();
//    ~Painter() override;
//
//    bool first{true};
//    bool m_Visibility{true};
//
//    Pen::Pointer m_Pen;
//    Brush::Pointer m_Brush;
//
//    GLVertexArray m_VAO;
//
//    std::vector<Vector3f> m_Points{};
//    std::vector<Vector3f> m_Colors{};
//    GLBuffer m_PositionVBO, m_ColorVBO;
//    IGsize m_CurrentVBOSize, m_MaximumVBOSize;
//
//    std::vector<iguIndex> m_TriangleIndexes{};
//    GLBuffer m_TriangleEBO;
//    IGsize m_CurrentTriangleEBOSize, m_MaximumTriangleEBOSize;
//
//    IndexMapper m_PointIndexesMapper{};
//    IndexMapper m_LineIndexesMapper{};
//    EBOMapper m_PointEBOMapper, m_LineEBOMapper;
//    EBOSizeMapper m_CurrentPointEBOSizeMapper, m_MaximumPointEBOSizeMapper;
//    EBOSizeMapper m_CurrentLineEBOSizeMapper, m_MaximumLineEBOSizeMapper;
//
//    HandlePool m_HandlePool;
//};
//
//IGAME_NAMESPACE_END