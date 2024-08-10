#include "iGameLinePainter.h"
#include "iGameScene.h"

IGAME_NAMESPACE_BEGIN
void LinePainter::DrawLine(const Point& p1, const Point& p2, Color c) {
    m_Points->AddPoint(p1);
    m_Points->AddPoint(p2);
    m_Colors->AddElement(ColorMap(c));
    m_Colors->AddElement(ColorMap(c));
    this->Modified();
}

void LinePainter::Clear() {
    m_Points->Initialize();
    m_Colors->Initialize();
    this->Modified();
}

void LinePainter::Draw(Scene* scene) {
    if (!m_Visibility) { return; }

    if (!m_Points || m_Points->GetMTime() < this->GetMTime()) {
        VAO.destroy();
        pVBO.destroy();
        cVBO.destroy();

        VAO.create();
        pVBO.create();
        cVBO.create();

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

        m_Points->Modified();
    }

    scene->UBO().useColor = true;
    scene->UpdateUniformBuffer();

    scene->GetShader(Scene::NOLIGHT)->use();
    VAO.bind();
    glad_glLineWidth(m_LineWidth);
    glad_glDepthRange(0, 0.999999);
    glad_glDrawArrays(GL_LINES, 0, m_Points->GetNumberOfPoints());
    glad_glDepthRange(0, 1);
    VAO.release();
}

LinePainter::LinePainter() {
    m_Points = Points::New();
    m_Colors = FloatArray::New();
    m_Colors->SetElementSize(3);
}
IGAME_NAMESPACE_END