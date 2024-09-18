//#include "iGameFacePainter.h"
//#include "iGameScene.h"
//
//IGAME_NAMESPACE_BEGIN
//
//void FacePainter::DrawTriangle(const Point& p1, const Point& p2,
//                               const Point& p3, Color c)
//{
//    m_Points->AddPoint(p1);
//    m_Points->AddPoint(p2);
//    m_Points->AddPoint(p3);
//    m_Colors->AddElement(ColorMap(c));
//    m_Colors->AddElement(ColorMap(c));
//    m_Colors->AddElement(ColorMap(c));
//}
//
//void FacePainter::Clear() {
//    m_Points->Initialize();
//    m_Colors->Initialize();
//}
//
//void FacePainter::Draw(Scene* scene) {
//    if (!m_Visibility) { return; }
//
//    if (m_Points == nullptr) {
//        return;
//    }
//
//    if (this->GetMTime() < m_Points->GetMTime()) {
//        VAO.destroy();
//        pVBO.destroy();
//        cVBO.destroy();
//
//        VAO.create();
//        pVBO.create();
//        pVBO.target(GL_ARRAY_BUFFER);
//        cVBO.create();
//        cVBO.target(GL_ARRAY_BUFFER);
//
//        GLAllocateGLBuffer(pVBO,
//                           m_Points->GetNumberOfPoints() * 3 * sizeof(float),
//                           m_Points->RawPointer());
//        GLAllocateGLBuffer(cVBO, m_Colors->GetNumberOfValues() * sizeof(float),
//                           m_Colors->RawPointer());
//
//        VAO.vertexBuffer(GL_VBO_IDX_0, pVBO, 0, 3 * sizeof(float));
//        GLSetVertexAttrib(VAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
//                          GL_FALSE, 0);
//
//        VAO.vertexBuffer(GL_VBO_IDX_1, cVBO, 0, 3 * sizeof(float));
//        GLSetVertexAttrib(VAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
//                          GL_FALSE, 0);
//
//        this->Modified();
//    }
//
//    if (!VAO) {return;}
//    scene->UseColor();
//    scene->UpdateUniformBuffer();
//    scene->UseShader(Scene::PATCH);
//    VAO.bind();
//    glad_glDepthRange(0.00001, 1);
//    glad_glDrawArrays(GL_TRIANGLES, 0, m_Points->GetNumberOfPoints());
//    glad_glDepthRange(0, 1);
//    VAO.release();
//}
//
//FacePainter::FacePainter() {
//    m_Points = Points::New();
//    m_Colors = FloatArray::New();
//    m_Colors->SetElementSize(3);
//}
//IGAME_NAMESPACE_END