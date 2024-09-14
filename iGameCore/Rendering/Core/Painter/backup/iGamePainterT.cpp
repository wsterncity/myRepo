////
//// Created by Sumzeek on 9/12/2024.
////
//
//#include "iGamePainter.h"
//#include "iGameScene.h"
//
//IGAME_NAMESPACE_BEGIN
//
//Painter::Painter() {
//    m_Pen = Pen::New();
//    m_Brush = Brush::New();
//    Clear();
//}
//
//Painter::~Painter() {}
//
//void Painter::ShowAll() { m_Visibility = true; }
//void Painter::HideAll() { m_Visibility = false; }
//void Painter::Show(IGuint handle) {
//    m_HandlePool.CheckHandle(handle);
//    ;
//}
//void Painter::Hide(IGuint handle) { m_HandlePool.CheckHandle(handle); }
//
//void Painter::SetPen(const Pen::Pointer& pen) { m_Pen = pen; }
//void Painter::SetPen(const Color& color) { m_Pen->SetColor(color); }
//void Painter::SetPen(const PenStyle& style) { m_Pen->SetStyle(style); }
//void Painter::SetPen(int width) { m_Pen->SetWidth(width); }
//
//void Painter::SetBrush(const Color& color) { m_Brush->SetColor(color); }
//void Painter::SetBrush(const Brush::Pointer& brush) { m_Brush = brush; }
//void Painter::SetBrush(const BrushStyle& style) { m_Brush->SetStyle(style); }
//
//IGuint Painter::DrawPoint(const Point& point) {
//    if (m_Pen->GetColor() == Color::None) { return 0; }
//
//    m_Points.emplace_back(point);
//    m_Colors.emplace_back(ColorMap(m_Pen->GetColor()));
//
//    iguIndex index = static_cast<iguIndex>(m_Points.size() - 1);
//
//    int width = m_Pen->GetWidth();
//    auto& indexes = m_PointIndexesMapper[width];
//    indexes.emplace_back(index);
//
//    PenWidthUpdate();
//    return m_HandlePool.AllocateHandle();
//}
//
//IGuint Painter::DrawLine(const Point& p1, const Point& p2) {
//    if (m_Pen->GetColor() == Color::None) { return 0; }
//
//    m_Points.emplace_back(p1);
//    m_Points.emplace_back(p2);
//    m_Colors.emplace_back(ColorMap(m_Pen->GetColor()));
//    m_Colors.emplace_back(ColorMap(m_Pen->GetColor()));
//
//    iguIndex index1 = static_cast<iguIndex>(m_Points.size() - 1);
//    iguIndex index2 = static_cast<iguIndex>(m_Points.size() - 2);
//
//    PenWidthUpdate();
//    int width = m_Pen->GetWidth();
//    auto& indexes = m_LineIndexesMapper[width];
//    indexes.insert(indexes.end(), {index1, index2});
//
//    PenWidthUpdate();
//    return m_HandlePool.AllocateHandle();
//}
//
//IGuint Painter::DrawTriangle(const Point& p1, const Point& p2,
//                             const Point& p3) {
//    if (m_Pen->GetColor() == Color::None &&
//        m_Brush->GetColor() == Color::None) {
//        return 0;
//    }
//
//    if (m_Pen->GetColor() != Color::None) {
//        m_Points.emplace_back(p1);
//        m_Points.emplace_back(p2);
//        m_Points.emplace_back(p3);
//        m_Colors.emplace_back(ColorMap(m_Pen->GetColor()));
//        m_Colors.emplace_back(ColorMap(m_Pen->GetColor()));
//        m_Colors.emplace_back(ColorMap(m_Pen->GetColor()));
//
//        iguIndex index1 = static_cast<iguIndex>(m_Points.size() - 1);
//        iguIndex index2 = static_cast<iguIndex>(m_Points.size() - 2);
//        iguIndex index3 = static_cast<iguIndex>(m_Points.size() - 3);
//
//        PenWidthUpdate();
//        int width = m_Pen->GetWidth();
//        auto& indexes = m_LineIndexesMapper[width];
//        indexes.insert(indexes.end(),
//                       {index1, index2, index2, index3, index3, index1});
//    }
//
//    if (m_Brush->GetColor() != Color::None) {
//        m_Points.emplace_back(p1);
//        m_Points.emplace_back(p2);
//        m_Points.emplace_back(p3);
//        m_Colors.emplace_back(ColorMap(m_Brush->GetColor()));
//        m_Colors.emplace_back(ColorMap(m_Brush->GetColor()));
//        m_Colors.emplace_back(ColorMap(m_Brush->GetColor()));
//
//        iguIndex index1 = static_cast<igIndex>(m_Points.size() - 1);
//        iguIndex index2 = static_cast<igIndex>(m_Points.size() - 2);
//        iguIndex index3 = static_cast<igIndex>(m_Points.size() - 3);
//        m_TriangleIndexes.insert(m_TriangleIndexes.end(),
//                                 {index1, index2, index3});
//    }
//
//    return m_HandlePool.AllocateHandle();
//}
//
//void Painter::Draw(Scene* scene) {
//    if (!m_Visibility) return;
//
//    if (first) {
//        m_VAO.create();
//
//        m_PositionVBO.create();
//        m_PositionVBO.target(GL_ARRAY_BUFFER);
//        m_ColorVBO.create();
//        m_ColorVBO.target(GL_ARRAY_BUFFER);
//
//        m_VAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
//        m_VAO.vertexBuffer(GL_VBO_IDX_1, m_ColorVBO, 0, 3 * sizeof(float));
//
//        GLSetVertexAttrib(m_VAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
//                          GL_FALSE, 0);
//        GLSetVertexAttrib(m_VAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
//                          GL_FALSE, 0);
//
//        first = false;
//    }
//
//    // Append Data to Buffer
//    IGsize vboSize = m_Points.size() * sizeof(Vector3f);
//    if (vboSize > m_MaximumVBOSize) {
//        IGsize newSize = m_CurrentVBOSize;
//        while (newSize < vboSize) { newSize *= 2; }
//        m_PositionVBO.allocate(vboSize, m_Points.data(), GL_STATIC_DRAW);
//        m_ColorVBO.allocate(vboSize, m_Colors.data(), GL_STATIC_DRAW);
//
//        m_CurrentVBOSize = vboSize;
//        m_MaximumVBOSize = newSize;
//    }
//
//    for (const auto& [width, indexes]: m_PointIndexesMapper) {
//        IGsize pointEBOSize = indexes.size() * sizeof(iguIndex);
//        if (pointEBOSize > m_MaximumPointEBOSizeMapper[width]) {
//            IGsize newSize = m_CurrentPointEBOSizeMapper[width];
//            while (newSize < pointEBOSize) { newSize *= 2; }
//
//            m_PointEBOMapper[width].destroy();
//            m_PointEBOMapper[width].create();
//            m_PointEBOMapper[width].target(GL_ELEMENT_ARRAY_BUFFER);
//            m_PointEBOMapper[width].allocate(pointEBOSize, indexes.data(),
//                                             GL_STATIC_DRAW);
//
//            m_CurrentPointEBOSizeMapper[width] = pointEBOSize;
//            m_MaximumPointEBOSizeMapper[width] = newSize;
//        }
//    }
//
//    for (const auto& [width, indexes]: m_LineIndexesMapper) {
//        IGsize lineEBOSize = indexes.size() * sizeof(iguIndex);
//        if (lineEBOSize > m_MaximumLineEBOSizeMapper[width]) {
//            IGsize newSize = m_CurrentLineEBOSizeMapper[width];
//            while (newSize < lineEBOSize) { newSize *= 2; }
//
//            m_LineEBOMapper[width].destroy();
//            m_LineEBOMapper[width].create();
//            m_LineEBOMapper[width].target(GL_ELEMENT_ARRAY_BUFFER);
//            m_LineEBOMapper[width].allocate(lineEBOSize, indexes.data(),
//                                            GL_STATIC_DRAW);
//
//            m_CurrentLineEBOSizeMapper[width] = lineEBOSize;
//            m_MaximumLineEBOSizeMapper[width] = newSize;
//        }
//    }
//
//    IGsize triangleEBOSize = m_TriangleIndexes.size() * sizeof(iguIndex);
//    if (triangleEBOSize > m_MaximumTriangleEBOSize) {
//        IGsize newSize = m_CurrentTriangleEBOSize;
//        while (newSize < triangleEBOSize) { newSize *= 2; }
//
//        m_TriangleEBO.destroy();
//        m_TriangleEBO.create();
//        m_TriangleEBO.target(GL_ELEMENT_ARRAY_BUFFER);
//        m_TriangleEBO.allocate(triangleEBOSize, m_TriangleIndexes.data(),
//                               GL_STATIC_DRAW);
//
//        m_CurrentTriangleEBOSize = triangleEBOSize;
//        m_MaximumTriangleEBOSize = newSize;
//    }
//
//    scene->UBO().useColor = true;
//    scene->UpdateUniformBuffer();
//    scene->GetShader(Scene::NOLIGHT)->use();
//
//    m_VAO.bind();
//    glad_glDepthRange(0.000001, 1);
//
//    // draw points
//    for (const auto& [width, indexes]: m_PointIndexesMapper) {
//        auto& EBO = m_PointEBOMapper[width];
//        m_VAO.elementBuffer(EBO);
//        glad_glPointSize(width);
//        glad_glDrawElements(GL_POINTS, indexes.size(), GL_UNSIGNED_INT, 0);
//    }
//
//    // draw lines
//    for (const auto& [width, indexes]: m_LineIndexesMapper) {
//        auto& EBO = m_LineEBOMapper[width];
//        m_VAO.elementBuffer(EBO);
//        glad_glLineWidth(width);
//        glad_glDrawElements(GL_LINES, indexes.size(), GL_UNSIGNED_INT, 0);
//    }
//
//    // draw triangles
//    m_VAO.elementBuffer(m_TriangleEBO);
//    glad_glDrawElements(GL_TRIANGLES, m_TriangleIndexes.size(), GL_UNSIGNED_INT,
//                        0);
//
//    glad_glDepthRange(0, 1);
//    m_VAO.release();
//}
//
//void Painter::Clear() {
//    m_CurrentVBOSize = 1;
//    m_MaximumVBOSize = 0;
//
//    m_CurrentTriangleEBOSize = 1;
//    m_MaximumTriangleEBOSize = 0;
//
//    auto width = m_Pen->GetWidth();
//
//    m_CurrentPointEBOSizeMapper[width] = 1;
//    m_MaximumPointEBOSizeMapper[width] = 0;
//
//    m_CurrentLineEBOSizeMapper[width] = 1;
//    m_MaximumLineEBOSizeMapper[width] = 0;
//
//    m_Points.clear();
//    m_Colors.clear();
//    m_PointIndexesMapper.clear();
//    m_LineIndexesMapper.clear();
//    m_TriangleIndexes.clear();
//}
//
//void Painter::PenWidthUpdate() {
//    auto width = m_Pen->GetWidth();
//
//    if (m_CurrentPointEBOSizeMapper.find(width) !=
//        m_CurrentPointEBOSizeMapper.end())
//        return;
//
//    m_CurrentPointEBOSizeMapper[width] = 1;
//    m_CurrentLineEBOSizeMapper[width] = 1;
//
//    m_MaximumPointEBOSizeMapper[width] = 0;
//    m_MaximumLineEBOSizeMapper[width] = 0;
//}
//
//Vector3f Painter::ColorMap(Color color) {
//    switch (color) {
//        case Color::Red:
//            return Vector3f{1.0f, 0.0f, 0.0f};
//        case Color::Green:
//            return Vector3f{0.0f, 1.0f, 0.0f};
//        case Color::Blue:
//            return Vector3f{0.0f, 0.0f, 1.0f};
//        case Color::LightBlue:
//            return Vector3f{0.678f, 0.847f, 0.902f};
//        case Color::White:
//            return Vector3f{1.0f, 1.0f, 1.0f};
//        case Color::Black:
//            return Vector3f{0.0f, 0.0f, 0.0f};
//        case Color::Gray:
//            return Vector3f{0.5f, 0.5f, 0.5f};
//        case Color::DarkGray:
//            return Vector3f{0.25f, 0.25f, 0.25f};
//        case Color::LightGray:
//            return Vector3f{0.75f, 0.75f, 0.75f};
//        case Color::Yellow:
//            return Vector3f{1.0f, 1.0f, 0.0f};
//        case Color::Magenta:
//            return Vector3f{1.0f, 0.0f, 1.0f};
//        case Color::Cyan:
//            return Vector3f{0.0f, 1.0f, 1.0f};
//        case Color::Orange:
//            return Vector3f{1.0f, 0.647f, 0.0f};
//        case Color::Purple:
//            return Vector3f{0.5f, 0.0f, 0.5f};
//        case Color::Pink:
//            return Vector3f{1.0f, 0.752f, 0.796f};
//        case Color::Brown:
//            return Vector3f{0.647f, 0.165f, 0.165f};
//        case Color::Gold:
//            return Vector3f{1.0f, 0.843f, 0.0f};
//        default:
//            return Vector3f{0.0f, 0.0f, 0.0f};
//    }
//}
//
//void Painter::ExpandVBO(GLBuffer& vbo, size_t oldSize, size_t newSize) {
//    GLBuffer tmp;
//    tmp.create();
//    tmp.target(GL_ARRAY_BUFFER);
//    tmp.storage(newSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
//    GLBuffer::copySubData(vbo, tmp, 0, 0, oldSize);
//
//    vbo.destroy();
//    vbo = std::move(tmp);
//}
//
//void Painter::ExpandEBO(GLBuffer& ebo, size_t oldSize, size_t newSize) {
//    GLBuffer tmp;
//    tmp.create();
//    tmp.target(GL_ELEMENT_ARRAY_BUFFER);
//    tmp.storage(newSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
//    GLBuffer::copySubData(ebo, tmp, 0, 0, oldSize);
//
//    ebo.destroy();
//    ebo = std::move(tmp);
//}
//
//IGAME_NAMESPACE_END