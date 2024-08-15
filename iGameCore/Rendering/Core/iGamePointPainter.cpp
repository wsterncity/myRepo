#include "iGamePointPainter.h"
#include "iGameScene.h"

IGAME_NAMESPACE_BEGIN
void PointPainter::SetPointSize(int size) { m_PointSize = size; }

void PointPainter::DrawPoint(const Point& p, Color c) {
    m_Points->AddPoint(p);
    m_Colors->AddElement(ColorMap(c));
}

void PointPainter::Clear() {
    m_Points->Initialize();
    m_Colors->Initialize();
}

void PointPainter::Draw(Scene* scene) {
    if (!m_Visibility) { return; }

    if (m_Points == nullptr) { return; }

    if (this->GetMTime() < m_Points->GetMTime()) {
        VAO.destroy();
        pVBO.destroy();
        cVBO.destroy();

        VAO.create();
        pVBO.create();
        pVBO.target(GL_ARRAY_BUFFER);
        cVBO.create();
        cVBO.target(GL_ARRAY_BUFFER);

        GLAllocateGLBuffer(pVBO,
                           m_Points->GetNumberOfPoints() * 3 * sizeof(float),
                           m_Points->RawPointer());
        GLAllocateGLBuffer(cVBO, m_Colors->GetNumberOfValues() * sizeof(float),
                           m_Colors->RawPointer());

        VAO.vertexBuffer(GL_VBO_IDX_0, pVBO, 0, 3 * sizeof(float));
        GLSetVertexAttrib(VAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
                          GL_FALSE, 0);

        VAO.vertexBuffer(GL_VBO_IDX_1, cVBO, 0, 3 * sizeof(float));
        GLSetVertexAttrib(VAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
                          GL_FALSE, 0);

        this->Modified();
    }

    if (!VAO) return;

    scene->UBO().useColor = true;
    scene->UpdateUniformBuffer();
    scene->GetShader(Scene::NOLIGHT)->use();
    std::cout << m_Points << std::endl;
    VAO.bind();
    glad_glPointSize(m_PointSize);
    glad_glDepthRange(0, 0.9999);
    glad_glDrawArrays(GL_POINTS, 0, m_Points->GetNumberOfPoints());
    glad_glDepthRange(0, 1);
    VAO.release();
}

PointPainter::PointPainter() {
    m_Points = Points::New();
    m_Colors = FloatArray::New();
    m_Colors->SetElementSize(3);
}
IGAME_NAMESPACE_END
