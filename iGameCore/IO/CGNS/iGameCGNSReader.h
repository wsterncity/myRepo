#ifndef iGameCGNSReader_h
#define iGameCGNSReader_h

#include <iGameFilter.h>
#include <string> 
#include <iGameMacro.h>
#include <iGameVolumeMesh_2.h>
#include <iGameStructuredMesh.h>
#include <iGameUnStructuredMesh.h>
#include <cgns_io.h>
#include <cgnslib.h>
#include <unordered_set>
IGAME_NAMESPACE_BEGIN
class  iGameCGNSReader : public Filter {

public:
	I_OBJECT(iGameCGNSReader);
	static 	iGameCGNSReader* New() { return new iGameCGNSReader; }

	~iGameCGNSReader();
	DataObject::Pointer ReadFile(std::string name);

	void GenStructuredCellConnectivities(cgsize_t cellDim, cgsize_t* size);
	void ReadPointCoordinates(int pointNum, int positionDim, int index_file, int index_base, int index_zone, cgsize_t* size);
	void ReadUnstructuredCellConnectivities(int index_file, int index_base, int  index_zone, cgsize_t cellNum);
	void ReadFields(int index_file, int index_base, int  index_zone, int index_sol, ZoneType_t zoneType, int celldim, GridLocation_t location, cgsize_t* size);

	void ChangeMixElementToMyCell(std::vector<cgsize_t>, int);
	void TransformVectorArray();

	DataObject::Pointer GetOutput() {
		switch (this->m_DataObjectType)
		{
		case IG_NONE:
			return nullptr;
		case IG_STRUCTURED_MESH:
			m_StructuredMesh->SetPoints(m_Points);
			return m_StructuredMesh;
		case IG_UNSTRUCTURED_MESH:
			m_UnstructuredMesh->SetPoints(m_Points);
			return m_UnstructuredMesh;
		case IG_VOLUME_MESH:
			m_VolumeMesh_2->SetPoints(m_Points);
			return m_VolumeMesh_2;
		default:
			return nullptr;
		}
	}
private:
	iGameCGNSReader();
	Points::Pointer m_Points{ nullptr };
	StructuredMesh::Pointer m_StructuredMesh{ nullptr };
	UnstructuredMesh::Pointer m_UnstructuredMesh{ nullptr };
	VolumeMesh_2::Pointer m_VolumeMesh_2{ nullptr };
	AttributeSet::Pointer m_AttributeSet{ nullptr };
	igIndex m_DataObjectType = IG_NONE;
	std::unordered_set<std::string >BoundryNames;
};

#endif // iGameCGNSReader_h
IGAME_NAMESPACE_END