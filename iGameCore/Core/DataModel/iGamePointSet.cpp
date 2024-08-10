#include "iGamePointSet.h"
#include "iGameScene.h"

IGAME_NAMESPACE_BEGIN
void PointSet::SetPoints(Points::Pointer points) {
  if (m_Points != points) {
    m_Points = points;
    this->Modified();
  }
}
Points::Pointer PointSet::GetPoints() { return m_Points; }

IGsize PointSet::GetNumberOfPoints() {
  return m_Points ? m_Points->GetNumberOfPoints() : 0;
}

const Point &PointSet::GetPoint(const IGsize ptId) const {
  return m_Points->GetPoint(ptId);
}

void PointSet::SetPoint(const IGsize ptId, const Point &p) {
  m_Points->SetPoint(ptId, p);
}

IGsize PointSet::AddPoint(const Point &p) {
  if (!InEditStatus()) {
    RequestEditStatus();
  }
  IGsize id = m_Points->AddPoint(p);
  m_PointDeleteMarker->AddTag();
  return id;
}

void PointSet::RequestEditStatus() {
  if (InEditStatus()) {
    return;
  }
  RequestPointStatus();
  MakeEditStatusOn();
}

void PointSet::DeletePoint(const IGsize ptId) {
  if (!InEditStatus()) {
    RequestEditStatus();
  }
  m_PointDeleteMarker->MarkDeleted(ptId);
}

bool PointSet::IsPointDeleted(const IGsize ptId) {
  return m_PointDeleteMarker->IsDeleted(ptId);
}

void PointSet::GarbageCollection() {
  IGsize i, mapId = 0;
  for (i = 0; i < GetNumberOfPoints(); i++) {
    if (IsPointDeleted(i))
      continue;
    if (i != mapId) {
      m_Points->SetPoint(mapId, m_Points->GetPoint(i));
    }
    mapId++;
  }
  m_Points->Resize(mapId);

  m_PointDeleteMarker = nullptr;
  Modified();
  MakeEditStatusOff();
}

bool PointSet::InEditStatus() { return m_InEditStatus; }
void PointSet::MakeEditStatusOn() { m_InEditStatus = true; }
void PointSet::MakeEditStatusOff() { m_InEditStatus = false; }

PointSet::PointSet() {
  m_Points = Points::New();
  m_ViewStyle = IG_POINT;
}

void PointSet::RequestPointStatus() {
  if (m_PointDeleteMarker == nullptr) {
    m_PointDeleteMarker = DeleteMarker::New();
  }
  m_PointDeleteMarker->Initialize(this->GetNumberOfPoints());
}

void PointSet::ComputeBoundingBox() {
  // std::cout << m_BoundingHelper->GetMTime() << " " << m_Points->GetMTime() <<
  // std::endl;
  if (m_Bounding.isNull() ||
      m_BoundingHelper->GetMTime() < m_Points->GetMTime()) {
    m_Bounding.reset();
    for (int i = 0; i < GetNumberOfPoints(); i++) {
      m_Bounding.add(GetPoint(i));
    }
    m_BoundingHelper->Modified();
  }
}

void PointSet::Draw(Scene *scene) {
  if (!m_Visibility) {
    return;
  }

  if (m_UseColor) {
    scene->UBO().useColor = true;
  } else {
    scene->UBO().useColor = false;
  }
  scene->UpdateUniformBuffer();

  if (m_ViewStyle == IG_POINTS) {
    scene->GetShader(Scene::NOLIGHT)->use();
    m_PointVAO.bind();
    glPointSize(m_PointSize);
    glad_glDrawArrays(GL_POINTS, 0, m_Positions->GetNumberOfValues() / 3);
    m_PointVAO.release();
  }
}
// TODO: wait to implement
void PointSet::DrawPhase1(Scene *) {

};
void PointSet::DrawPhase2(Scene *) {

};
void PointSet::TestOcclusionResults(Scene *) {

};

void PointSet::ConvertToDrawableData() {
  if (m_Positions && m_Positions->GetMTime() > this->GetMTime()) {
    return;
  }
  // if (m_Positions)
  //{
  //	std::cout << m_Positions->GetMTime() << " " << this->GetMTime() <<
  //std::endl;
  // }

  m_Positions = m_Points->ConvertToArray();

  m_PointVAO.destroy();
  m_PositionVBO.destroy();
  m_ColorVBO.destroy();
  m_PointEBO.destroy();

  m_PointVAO.create();
  m_PositionVBO.create();
  m_ColorVBO.create();
  m_PointEBO.create();

  GLAllocateGLBuffer(m_PositionVBO,
                     m_Positions->GetNumberOfValues() * sizeof(float),
                     m_Positions->RawPointer());

  m_PointVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
  GLSetVertexAttrib(m_PointVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
                    GL_FALSE, 0);

  m_Positions->Modified();
}

void PointSet::ViewCloudPicture(int index, int demension) {
  auto &attr = this->GetPropertySet()->GetProperty(index);
  if (!attr.isDeleted && attr.attachmentType == IG_POINT) {
    this->SetAttributeWithPointData(attr.pointer, demension);
  }
}

void PointSet::SetAttributeWithPointData(ArrayObject::Pointer attr, igIndex i) {
  if (m_ViewAttribute != attr || m_ViewDemension != i) {
    if (attr == nullptr) {
      m_UseColor = false;
      m_ViewAttribute = nullptr;
      m_ViewDemension = -1;
      return;
    }
    m_ViewAttribute = attr;
    m_ViewDemension = i;

    m_UseColor = true;
    ScalarsToColors::Pointer mapper = ScalarsToColors::New();

    if (i == -1) {
      mapper->InitRange(attr);
    } else {
      mapper->InitRange(attr, i);
    }

    m_Colors = mapper->MapScalars(attr, i);
    if (m_Colors == nullptr) {
      return;
    }

    GLAllocateGLBuffer(m_ColorVBO,
                       m_Colors->GetNumberOfValues() * sizeof(float),
                       m_Colors->RawPointer());

    m_PointVAO.vertexBuffer(GL_VBO_IDX_1, m_ColorVBO, 0, 3 * sizeof(float));
    GLSetVertexAttrib(m_PointVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
                      GL_FALSE, 0);
  }
}
IGAME_NAMESPACE_END