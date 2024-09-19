//
// Created by Sumzeek on 9/12/2024.
//

#include "iGamePainter.h"
#include "iGameScene.h"

IGAME_NAMESPACE_BEGIN

Painter::Painter() {
    m_Pen = Pen::New();
    m_Brush = Brush::New();
    Clear();
}

Painter::~Painter() {}

void Painter::ShowAll() {
    for (auto& [handle, primitive]: m_PrimitivesPool) {
        primitive.visible = true;
    }
}
void Painter::HideAll() {
    for (auto& [handle, primitive]: m_PrimitivesPool) {
        primitive.visible = false;
    }
}
void Painter::Show(IGuint handle) {
    m_PrimitivesPool.CheckHandle(handle);

    auto primitive = m_PrimitivesPool.GetObject(handle);
    primitive.visible = true;
}
void Painter::Hide(IGuint handle) {
    m_PrimitivesPool.CheckHandle(handle);

    auto& primitive = m_PrimitivesPool.GetObject(handle);
    primitive.visible = false;
}

void Painter::Delete(IGuint handle) {
    m_PrimitivesPool.CheckHandle(handle);
    m_PrimitivesPool.ReleaseHandle(handle);
}

void Painter::SetPen(const Pen::Pointer& pen) { m_Pen = pen; }
void Painter::SetPen(const Color& color) { m_Pen->SetColor(color); }
void Painter::SetPen(const PenStyle& style) { m_Pen->SetStyle(style); }
void Painter::SetPen(int width) { m_Pen->SetWidth(width); }

void Painter::SetBrush(const Color& color) { m_Brush->SetColor(color); }
void Painter::SetBrush(const Brush::Pointer& brush) { m_Brush = brush; }
void Painter::SetBrush(const BrushStyle& style) { m_Brush->SetStyle(style); }

IGuint Painter::DrawPoint(const Point& point) {
    if (m_Pen->GetColor() == Color::None) { return 0; }

    Primitive primitive{};
    primitive.penWidth = m_Pen->GetWidth();

    auto& points = primitive.points;
    auto& colors = primitive.colors;
    auto& indexes = primitive.indexes;

    auto c = ColorMap(m_Pen->GetColor());
    points.insert(points.end(), {point});
    colors.insert(colors.end(), {c});

    std::vector<int> index = {0};
    indexes[0].insert(indexes[0].end(), index.begin(), index.end());

    auto handle = m_PrimitivesPool.AllocateObject(primitive);
    return handle;
}

IGuint Painter::DrawLine(const Point& p1, const Point& p2) {
    if (m_Pen->GetColor() == Color::None) { return 0; }

    Primitive primitive{};
    primitive.penWidth = m_Pen->GetWidth();

    auto& points = primitive.points;
    auto& colors = primitive.colors;
    auto& indexes = primitive.indexes;

    auto color = ColorMap(m_Pen->GetColor());
    points.insert(points.end(), {p1, p2});
    colors.insert(colors.end(), {color, color});

    std::vector<int> index = {0, 1};
    indexes[1].insert(indexes[1].end(), index.begin(), index.end());

    auto handle = m_PrimitivesPool.AllocateObject(primitive);
    return handle;
}

IGuint Painter::DrawTriangle(const Point& p1, const Point& p2,
                             const Point& p3) {
    if (m_Pen->GetColor() == Color::None &&
        m_Brush->GetColor() == Color::None) {
        return 0;
    }

    Primitive primitive{};
    primitive.penWidth = m_Pen->GetWidth();

    auto& points = primitive.points;
    auto& colors = primitive.colors;
    auto& indexes = primitive.indexes;

    if (m_Pen->GetColor() != Color::None) {
        auto color = ColorMap(m_Pen->GetColor());
        points.insert(points.end(), {p1, p2, p3});
        colors.insert(colors.end(), {color, color, color});

        std::vector<int> index = {0, 1, 1, 2, 2, 0};
        indexes[1].insert(indexes[1].end(), index.begin(), index.end());
    }

    if (m_Brush->GetColor() != Color::None) {
        auto color = ColorMap(m_Brush->GetColor());
        points.insert(points.end(), {p1, p2, p3});
        colors.insert(colors.end(), {color, color, color});

        std::vector<int> index = {3, 4, 5};
        indexes[2].insert(indexes[2].end(), index.begin(), index.end());
    }

    auto handle = m_PrimitivesPool.AllocateObject(primitive);
    return handle;
}

IGuint Painter::DrawRect(const Point& p1, const Point& p3) {
    if (m_Pen->GetColor() == Color::None &&
        m_Brush->GetColor() == Color::None) {
        return 0;
    }

    Primitive primitive{};
    primitive.penWidth = m_Pen->GetWidth();

    auto& points = primitive.points;
    auto& colors = primitive.colors;
    auto& indexes = primitive.indexes;

    auto p2 = Point{p1[0], p3[1], p1[2]};
    auto p4 = Point{p3[0], p1[1], p3[2]};

    if (m_Pen->GetColor() != Color::None) {
        auto color = ColorMap(m_Pen->GetColor());
        points.insert(points.end(), {p1, p2, p3, p4});
        colors.insert(colors.end(), {color, color, color, color});

        std::vector<int> index = {0, 1, 1, 2, 2, 3, 3, 0};
        indexes[1].insert(indexes[1].end(), index.begin(), index.end());
    }

    if (m_Brush->GetColor() != Color::None) {
        auto color = ColorMap(m_Brush->GetColor());
        points.insert(points.end(), {p1, p2, p3, p4});
        colors.insert(colors.end(), {color, color, color, color});

        std::vector<int> index = {4, 5, 6, 6, 7, 4};
        indexes[2].insert(indexes[2].end(), index.begin(), index.end());
    }

    auto handle = m_PrimitivesPool.AllocateObject(primitive);
    return handle;
}

IGuint Painter::DrawCube(const Point& p1, const Point& p7) {
    if (m_Pen->GetColor() == Color::None &&
        m_Brush->GetColor() == Color::None) {
        return 0;
    }

    Primitive primitive{};
    primitive.penWidth = m_Pen->GetWidth();

    auto& points = primitive.points;
    auto& colors = primitive.colors;
    auto& indexes = primitive.indexes;

    auto p2 = Point{p1[0], p1[1], p7[2]};
    auto p3 = Point{p7[0], p1[1], p7[2]};
    auto p4 = Point{p7[0], p1[1], p1[2]};
    auto p5 = Point{p1[0], p7[1], p1[2]};
    auto p6 = Point{p1[0], p7[1], p7[2]};
    auto p8 = Point{p7[0], p7[1], p1[2]};

    if (m_Pen->GetColor() != Color::None) {
        auto color = ColorMap(m_Pen->GetColor());
        points.insert(points.end(), {p1, p2, p3, p4, p5, p6, p7, p8});
        colors.insert(colors.end(),
                      {color, color, color, color, color, color, color, color});

        std::vector<int> index = {0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 5,
                                  2, 6, 3, 7, 4, 5, 5, 6, 6, 7, 7, 4};
        indexes[1].insert(indexes[1].end(), index.begin(), index.end());
    }

    if (m_Brush->GetColor() != Color::None) {
        auto color = ColorMap(m_Brush->GetColor());
        points.insert(points.end(), {p1, p2, p3, p4, p5, p6, p7, p8});
        colors.insert(colors.end(),
                      {color, color, color, color, color, color, color, color});

        std::vector<int> index = {8, 9,  10, 10, 11, 8, 8,  9,  13, 13, 12, 8,
                                  8, 11, 15, 15, 12, 8, 11, 15, 14, 14, 10, 11,
                                  9, 10, 14, 14, 13, 9, 12, 13, 14, 14, 15, 12};
        indexes[2].insert(indexes[2].end(), index.begin(), index.end());
    }

    auto handle = m_PrimitivesPool.AllocateObject(primitive);
    return handle;
}

void Painter::Draw(Scene* scene) {
    if (first) {
        m_VAO.create();

        m_PositionVBO.create();
        m_PositionVBO.target(GL_ARRAY_BUFFER);
        m_ColorVBO.create();
        m_ColorVBO.target(GL_ARRAY_BUFFER);
        m_PointEBO.create();
        m_PointEBO.target(GL_ELEMENT_ARRAY_BUFFER);
        m_LineEBO.create();
        m_LineEBO.target(GL_ELEMENT_ARRAY_BUFFER);
        m_TriangleEBO.create();
        m_TriangleEBO.target(GL_ELEMENT_ARRAY_BUFFER);

        m_VAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
        m_VAO.vertexBuffer(GL_VBO_IDX_1, m_ColorVBO, 0, 3 * sizeof(float));

        GLSetVertexAttrib(m_VAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
                          GL_FALSE, 0);
        GLSetVertexAttrib(m_VAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
                          GL_FALSE, 0);

        first = false;
    }

    for (auto& [handle, primitive]: m_PrimitivesPool) {
        if (!primitive.visible) { continue; }

        auto& points = primitive.points;
        auto& colors = primitive.colors;
        auto& indexes = primitive.indexes;

        m_PositionVBO.allocate(points.size() * sizeof(Vector3f), points.data(),
                               GL_STATIC_DRAW);
        m_ColorVBO.allocate(colors.size() * sizeof(Vector3f), colors.data(),
                            GL_STATIC_DRAW);

        scene->UBO().useColor = true;
        scene->UpdateUniformBuffer();
        scene->GetShader(Scene::NOLIGHT)->use();

        m_VAO.bind();
        glad_glDepthRange(0.000001, 1);

        if (indexes[0].size() != 0) {
            m_PointEBO.allocate(indexes[0].size() * sizeof(iguIndex),
                                indexes[0].data(), GL_STATIC_DRAW);
            m_VAO.elementBuffer(m_PointEBO);

            m_VAO.bind();
            glad_glPointSize(primitive.penWidth);
            glad_glDrawElements(GL_POINTS, indexes[0].size(), GL_UNSIGNED_INT,
                                0);
        }
        if (indexes[1].size() != 0) {
            m_LineEBO.allocate(indexes[1].size() * sizeof(iguIndex),
                               indexes[1].data(), GL_STATIC_DRAW);
            m_VAO.elementBuffer(m_LineEBO);

            m_VAO.bind();
            glad_glLineWidth(primitive.penWidth);
            glad_glDrawElements(GL_LINES, indexes[1].size(), GL_UNSIGNED_INT,
                                0);
        }
        if (indexes[2].size() != 0) {
            m_TriangleEBO.allocate(indexes[2].size() * sizeof(iguIndex),
                                   indexes[2].data(), GL_STATIC_DRAW);
            m_VAO.elementBuffer(m_TriangleEBO);

            m_VAO.bind();
            glad_glDrawElements(GL_TRIANGLES, indexes[2].size(),
                                GL_UNSIGNED_INT, 0);
        }

        glad_glDepthRange(0, 1);
        m_VAO.release();
    }
}

void Painter::Clear() {
    m_PrimitivesPool.Clear();
    
    m_Points.clear();
    m_Colors.clear();
    m_PointIndexes.clear();
    m_LineIndexes.clear();
    m_TriangleIndexes.clear();
}

Vector3f Painter::ColorMap(Color color) {
    switch (color) {
        case Color::Red:
            return Vector3f{1.0f, 0.0f, 0.0f};
        case Color::Green:
            return Vector3f{0.0f, 1.0f, 0.0f};
        case Color::Blue:
            return Vector3f{0.0f, 0.0f, 1.0f};
        case Color::LightBlue:
            return Vector3f{0.678f, 0.847f, 0.902f};
        case Color::White:
            return Vector3f{1.0f, 1.0f, 1.0f};
        case Color::Black:
            return Vector3f{0.0f, 0.0f, 0.0f};
        case Color::Gray:
            return Vector3f{0.5f, 0.5f, 0.5f};
        case Color::DarkGray:
            return Vector3f{0.25f, 0.25f, 0.25f};
        case Color::LightGray:
            return Vector3f{0.75f, 0.75f, 0.75f};
        case Color::Yellow:
            return Vector3f{1.0f, 1.0f, 0.0f};
        case Color::Magenta:
            return Vector3f{1.0f, 0.0f, 1.0f};
        case Color::Cyan:
            return Vector3f{0.0f, 1.0f, 1.0f};
        case Color::Orange:
            return Vector3f{1.0f, 0.647f, 0.0f};
        case Color::Purple:
            return Vector3f{0.5f, 0.0f, 0.5f};
        case Color::Pink:
            return Vector3f{1.0f, 0.752f, 0.796f};
        case Color::Brown:
            return Vector3f{0.647f, 0.165f, 0.165f};
        case Color::Gold:
            return Vector3f{1.0f, 0.843f, 0.0f};
        default:
            return Vector3f{0.0f, 0.0f, 0.0f};
    }
}

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

IGAME_NAMESPACE_END