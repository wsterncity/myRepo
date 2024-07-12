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
IGAME_NAMESPACE_END