#include "iGameDataObject.h"
#include "iGameSurfaceMesh.h"

IGAME_NAMESPACE_BEGIN
DataObject::Pointer DataObject::CreateDataObject(IGenum type) {
	switch (type)
	{
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

DataObject::Pointer DataObject::GetSubDataObject(DataObjectId id) 
{
	if (m_SubDataObjectsHelper == nullptr)
	{
		return nullptr;
	}

	return m_SubDataObjectsHelper->GetSubDataObject(id);
}

DataObjectId DataObject::AddSubDataObject(DataObject::Pointer obj)
{
	if (m_SubDataObjectsHelper == nullptr)
	{
		m_SubDataObjectsHelper = SubDataObjectsHelper::New();
	}

	return m_SubDataObjectsHelper->AddSubDataObject(obj);
}

void DataObject::RemoveSubDataObject(DataObjectId id)
{
	if (m_SubDataObjectsHelper == nullptr)
	{
		return;
	}

	return m_SubDataObjectsHelper->RemoveSubDataObject(id);
}

bool DataObject::HasSubDataObject() noexcept
{
	if (m_SubDataObjectsHelper == nullptr)
	{
		return false;
	}

	return m_SubDataObjectsHelper->HasSubDataObject();
}

int DataObject::GetNumberOfSubDataObjects() noexcept
{
	if (m_SubDataObjectsHelper == nullptr)
	{
		return 0;
	}

	return m_SubDataObjectsHelper->GetNumberOfSubDataObjects();
}

DataObject::SubIterator DataObject::SubBegin()
{
	return m_SubDataObjectsHelper->Begin();
}

DataObject::SubConstIterator DataObject::SubBegin() const
{
	return m_SubDataObjectsHelper->Begin();
}

DataObject::SubIterator DataObject::SubEnd()
{
	return m_SubDataObjectsHelper->End();
}

DataObject::SubConstIterator DataObject::SubEnd() const
{
	return m_SubDataObjectsHelper->End();
}

void DataObject::Draw(Scene* scene)
{
	ProcessSubDataObjects(&DataObject::Draw, scene);
}
void DataObject::ConvertToDrawableData()
{
	ProcessSubDataObjects(&DataObject::ConvertToDrawableData);
}
void DataObject::ViewCloudPicture(int index, int demension) // ¿ÉÊÓ»¯ÔÆÍ¼
{
	ProcessSubDataObjects(&DataObject::ViewCloudPicture, index, demension);
}
void DataObject::SetViewStyle(IGenum mode)
{
	m_ViewStyle = mode; 
	ProcessSubDataObjects(&DataObject::SetViewStyle, mode);
}
void DataObject::SetVisibility(bool f)
{
	this->m_Visibility = f;
	ProcessSubDataObjects(&DataObject::SetVisibility, f);
}
IGAME_NAMESPACE_END