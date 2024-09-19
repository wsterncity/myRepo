#include "iGameDataObject.h"
#include "iGameSurfaceMesh.h"

IGAME_NAMESPACE_BEGIN
DataObject::Pointer DataObject::CreateDataObject(IGenum type) {
  switch (type) {
  case IG_DATA_OBJECT:
    return DataObject::New();
  case IG_POINT_SET:
    return PointSet::New();
  case IG_SURFACE_MESH:
    return SurfaceMesh::New();
  default:
    return nullptr;
  }
}

DataObject::Pointer DataObject::GetSubDataObject(DataObjectId id) {
  if (m_SubDataObjectsHelper == nullptr) {
    return nullptr;
  }

  return m_SubDataObjectsHelper->GetSubDataObject(id);
}

DataObjectId DataObject::AddSubDataObject(DataObject::Pointer obj) {
  if (m_SubDataObjectsHelper == nullptr) {
    m_SubDataObjectsHelper = SubDataObjectsHelper::New();
  }
  obj->SetParent(this);
  return m_SubDataObjectsHelper->AddSubDataObject(obj);
}

void DataObject::RemoveSubDataObject(DataObjectId id) {
  if (m_SubDataObjectsHelper == nullptr) {
    return;
  }

  return m_SubDataObjectsHelper->RemoveSubDataObject(id);
}

void DataObject::ClearSubDataObject() {
  if (m_SubDataObjectsHelper == nullptr) {
    return;
  }

  return m_SubDataObjectsHelper->ClearSubDataObject();
}

bool DataObject::HasSubDataObject() noexcept {
  if (m_SubDataObjectsHelper == nullptr) {
    return false;
  }

  return m_SubDataObjectsHelper->HasSubDataObject();
}

int DataObject::GetNumberOfSubDataObjects() noexcept {
  if (m_SubDataObjectsHelper == nullptr) {
    return 0;
  }

  return m_SubDataObjectsHelper->GetNumberOfSubDataObjects();
}

DataObject::SubIterator DataObject::SubDataObjectIteratorBegin() {
  return m_SubDataObjectsHelper->Begin();
}

DataObject::SubConstIterator DataObject::SubDataObjectIteratorBegin() const {
  return m_SubDataObjectsHelper->Begin();
}

DataObject::SubIterator DataObject::SubDataObjectIteratorEnd() {
  return m_SubDataObjectsHelper->End();
}

DataObject *DataObject::FindParent() {
  if (m_Parent != nullptr) {
    return m_Parent->FindParent();
  }
  return this;
}

DataObject::SubConstIterator DataObject::SubDataObjectIteratorEnd() const {
  return m_SubDataObjectsHelper->End();
}

DataObjectId DataObject::GetIncrementDataObjectId() {
  static DataObjectId globalDataObjectId = 0;
  return globalDataObjectId++;
}
void DataObject::SetParent(DataObject *parent) {
  if (m_Parent != parent) {
    m_Parent = parent;
  }
}

void DataObject::Draw(Scene *scene) {
  ProcessSubDataObjects(&DataObject::Draw, scene);
}
void DataObject::DrawPhase1(Scene *scene) {
  ProcessSubDataObjects(&DataObject::DrawPhase1, scene);
}
void DataObject::DrawPhase2(Scene *scene) {
  ProcessSubDataObjects(&DataObject::DrawPhase2, scene);
}
void DataObject::TestOcclusionResults(Scene *scene) {
  ProcessSubDataObjects(&DataObject::TestOcclusionResults, scene);
}

void DataObject::ConvertToDrawableData() {
  ProcessSubDataObjects(&DataObject::ConvertToDrawableData);
}

void DataObject::ViewCloudPicture(Scene* scene,int index, int dimension) {
  m_AttributeIndex = index;
  m_AttributeDimension = dimension;
  ProcessSubDataObjects(&DataObject::ViewCloudPicture, scene, index, dimension);
}

void DataObject::ViewCloudPictureOfModel(Scene* scene, int index, int demension) {
  auto *parent = FindParent();
  if (parent != this) {
    parent->ViewCloudPicture(scene, index, demension);
  } else {
    this->ViewCloudPicture(scene, index, demension);
  }
}
void DataObject::SetViewStyle(IGenum mode) {
    /*
     * e.g. mode = IG_WIREFRAME | IG_SURFACE, means that the model shows the wireframe and surface.
     * */
    m_ViewStyle = mode;
    ProcessSubDataObjects(&DataObject::SetViewStyle, mode);
}

void DataObject::AddViewStyle(IGenum mode) {
    m_ViewStyle |= mode;
    ProcessSubDataObjects(&DataObject::AddViewStyle, mode);
}

void DataObject::RemoveViewStyle(IGenum mode)
{
    m_ViewStyle &= ~mode;
    ProcessSubDataObjects(&DataObject::RemoveViewStyle, mode);
}

void DataObject::AddViewStyleOfModel(IGenum mode) {
  auto *parent = FindParent();
  if (parent != this) {
    parent->AddViewStyle(mode);
  } else {
    this->AddViewStyle(mode);
  }
}

unsigned int DataObject::GetViewStyle() { return m_ViewStyle; }

unsigned int DataObject::GetViewStyleOfModel() {
    auto* parent = FindParent();
    if (parent != this) {
        return parent->GetViewStyle();
    } else {
        return this->GetViewStyle();
    }
}

void DataObject::SetVisibility(bool f) {
  this->m_Visibility = f;
  ProcessSubDataObjects(&DataObject::SetVisibility, f);
}

int DataObject::GetAttributeIndex() { return this->m_AttributeIndex; }

int DataObject::GetAttributeDimension() { return this->m_AttributeDimension; }

int DataObject::GetTimeframeIndex() { return this->m_CurrentTimeframeIndex; }

void DataObject::SwitchToCurrentTimeframe(int timeIndex) {
  if (m_TimeFrames == nullptr)
    igError("This operation cannot be performed in this file without time "
            "frames.");
  if (m_TimeFrames->GetArrays().size() <= timeIndex)
    igError("timeStep error");

  m_CurrentTimeframeIndex = timeIndex;
}

StreamingData::Pointer DataObject::GetTimeFrames() {
  if (m_TimeFrames == nullptr)
    m_TimeFrames = StreamingData::New();

  return m_TimeFrames;
}

//void DataObject::UpdateAttributeSetRange() {
//    IGsize scalarNum = this->GetAttributeSet()->GetAllAttributes()->GetNumberOfElements();
//    float range_max, range_min;
//    for(IGsize k = 0; k < scalarNum; k ++)
//    {
//        range_max = FLT_MIN;
//        range_min = FLT_MAX;
//        if(this->HasSubDataObject()){
//            this->
//        }
//        if(scalar_exist_1){
//            for(auto it = m_data_object->SubDataObjectIteratorBegin(); it != m_data_object->SubDataObjectIteratorEnd(); ++ it){
//                const auto& ScalarDataRange = it->second->GetAttributeSet()->GetAttribute(k).dataRange;
//                range_min = std::min(range_min, ScalarDataRange.first );
//                range_max = std::max(range_max, ScalarDataRange.second);
//            }
//            for(auto it = m_data_object->SubDataObjectIteratorBegin(); it != m_data_object->SubDataObjectIteratorEnd(); ++ it){
//                auto& ScalarDataRange = it->second->GetAttributeSet()->GetAttribute(k).dataRange;
//                ScalarDataRange.first  = range_min;
//                ScalarDataRange.second = range_max;
//            }
//        }
//        m_data_object->GetAttributeSet()->AddScalar(IG_POINT, array, {range_min, range_max});
//    }
//}

IGAME_NAMESPACE_END