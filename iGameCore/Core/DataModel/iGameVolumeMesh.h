#ifndef iGameVolumeMesh_h
#define iGameVolumeMesh_h

#include "iGameSurfaceMesh.h"

#include "iGameTetra.h"
#include "iGameHexahedron.h"
#include "iGamePrism.h"
#include "iGamePyramid.h"

IGAME_NAMESPACE_BEGIN
class VolumeMesh : public SurfaceMesh {
public:
	I_OBJECT(VolumeMesh);
	static Pointer New() { return new VolumeMesh; }

	// Get the number of all volumes
	IGsize GetNumberOfVolumes() const noexcept;
	
	// Get/Set volume array
	CellArray* GetVolumes();
	void SetVolumes(CellArray::Pointer volumes);

	// Get edge cell by index volumeId
	Volume* GetVolume(const IGsize volumeId);

	// Get volume's point index. Return PointIds size
	int GetVolumePointIds(const IGsize volumeId, igIndex* ptIds);
    // Get volume's edge index. Return EdgeIds size
	int GetVolumeEdgeIds(const IGsize volumeId, igIndex* edgeIds);
    // Get volume's face index. Return FaceIds size
	int GetVolumeFaceIds(const IGsize volumeId, igIndex* faceIds);

	// Construct all the faces and add the face index to VolumeFaces
	void BuildFaces();
    // Construct all the faces and edges. Add the face index to VolumeFaces.
	// Add the edge index to VolumeEdges,
    void BuildFacesAndEdges();
    // Construct the adjacent volumes of the points
	void BuildVolumeLinks();
    // Construct the adjacent volumes of the edges
	void BuildVolumeEdgeLinks();
    // Construct the adjacent volumes of the faces
	void BuildVolumeFaceLinks();

	// Get all neighboring volumes of a point. Return the size of indices.
	int GetPointToNeighborVolumes(const IGsize ptId, igIndex* volumeIds);
    // Get all neighboring volumes of a edge. Return the size of indices.
	int GetEdgeToNeighborVolumes(const IGsize edgeId, igIndex* volumeIds);
    // Get all neighboring volumes of a face. Return the size of indices.
	int GetFaceToNeighborVolumes(const IGsize faceId, igIndex* volumeIds);
    // Get all neighboring volumes of a volume (Shared point). Return the size of indices.
	int GetVolumeToNeighborVolumesWithPoint(const IGsize volumeId, igIndex* volumeIds);
    // Get all neighboring volumes of a volume (Shared edge). Return the size of indices.
	int GetVolumeToNeighborVolumesWithEdge(const IGsize volumeId, igIndex* volumeIds);
    // Get all neighboring volumes of a volume (Shared face). Return the size of indices.
	int GetVolumeToNeighborVolumesWithFace(const IGsize volumeId, igIndex* volumeIds);

	// Get volume index according to sequence. If don't, return index -1
	igIndex GetVolumeIdFormPointIds(igIndex* ids, int size);

	// Request data edit state, only in this state,
    // can perform the adding and delete operation.
    // Adding operations also can be done via GetVolumes().
	void RequestEditStatus() override;

    // Garbage collection to free memory that has been removed.
    // This function must be called if the topology changes.
	void GarbageCollection() override;

	// Whether volume is deleted or not
	bool IsVolumeDeleted(const IGsize volumeId);

	// Add element, necessarily called after RequestEditStatus()
	IGsize AddPoint(const Point& p) override;
	IGsize AddEdge(const IGsize ptId1, const IGsize ptId2) override;
	IGsize AddFace(igIndex* ptIds, int size) override;

	// Delete element, necessarily called after RequestEditStatus()
	void DeletePoint(const IGsize ptId) override;
	void DeleteEdge(const IGsize edgeId) override;
	void DeleteFace(const IGsize faceId) override;
	void DeleteVolume(const IGsize volumeId);

protected:
	VolumeMesh();
	~VolumeMesh() override = default;

	void RequestFaceStatus();
	void RequestVolumeStatus();

	DeleteMarker::Pointer m_VolumeDeleteMarker{};

	CellArray::Pointer m_Volumes{};         // The point set of volumes
	CellLinks::Pointer m_VolumeLinks{};     // The adjacent volumes of points
										    
	CellArray::Pointer m_VolumeEdges{};     // The edge set of volumes
	CellLinks::Pointer m_VolumeEdgeLinks{}; // The adjacent volumes of edges
										    
	CellArray::Pointer m_VolumeFaces{};     // The face set of volumes
	CellLinks::Pointer m_VolumeFaceLinks{}; // The adjacent volumes of faces

private:
	Tetra::Pointer m_Tetra{};            // Used for the returned 'Tetra' object, which is Thread-Unsafe
	Hexahedron::Pointer m_Hexahedron{};  // Used for the returned 'Hexahedron' object, which is Thread-Unsafe
	Prism::Pointer m_Prism{};            // Used for the returned 'Prism' object, which is Thread-Unsafe
	Pyramid::Pointer m_Pyramid{};        // Used for the returned 'Pyramid' object, which is Thread-Unsafe

public:
	void Draw(Scene*) override;
	void ConvertToDrawableData() override;
	bool IsDrawable() override { return true; }
	void ViewCloudPicture(int index, int demension = -1) override
	{
		if (index == -1)
		{
			m_UseColor = false;
			m_ViewAttribute = nullptr;
			m_ViewDemension = -1;
			m_ColorWithCell = false;
			return;
		}
		auto& attr = this->GetPropertySet()->GetProperty(index);
		if (!attr.isDeleted)
		{
			if (attr.attachmentType == IG_POINT)
				this->SetAttributeWithPointData(attr.pointer, demension);
			else if (attr.attachmentType == IG_CELL)
				this->SetAttributeWithCellData(attr.pointer, demension);
		}
	}

	void SetAttributeWithPointData(ArrayObject::Pointer attr, igIndex i = -1) override
	{
		if (m_ViewAttribute != attr || m_ViewDemension != i)
		{
			m_ViewAttribute = attr;
			m_ViewDemension = i;
			m_UseColor = true;
			m_ColorWithCell = false;
			ScalarsToColors::Pointer mapper = ScalarsToColors::New();

			if (i == -1)
			{
				mapper->InitRange(attr);
			}
			else
			{
				mapper->InitRange(attr, i);
			}

			m_Colors = mapper->MapScalars(attr, i);
			if (m_Colors == nullptr)
			{
				return;
			}

			GLAllocateGLBuffer(m_ColorVBO,
				m_Colors->GetNumberOfValues() * sizeof(float),
				m_Colors->RawPointer());

			m_PointVAO.vertexBuffer(GL_VBO_IDX_1, m_ColorVBO, 0, 3 * sizeof(float));
			GLSetVertexAttrib(m_PointVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
				GL_FALSE, 0);

			m_LineVAO.vertexBuffer(GL_VBO_IDX_1, m_ColorVBO, 0, 3 * sizeof(float));
			GLSetVertexAttrib(m_LineVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
				GL_FALSE, 0);


			m_TriangleVAO.vertexBuffer(GL_VBO_IDX_1, m_ColorVBO, 0, 3 * sizeof(float));
			GLSetVertexAttrib(m_TriangleVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
				GL_FALSE, 0);
		}
	}

	void SetAttributeWithCellData(ArrayObject::Pointer attr, igIndex i = -1)
	{
		if (m_ViewAttribute != attr || m_ViewDemension != i)
		{
			m_ViewAttribute = attr;
			m_ViewDemension = i;
			m_UseColor = true;
			m_ColorWithCell = true;
			ScalarsToColors::Pointer mapper = ScalarsToColors::New();

			if (i == -1)
			{
				mapper->InitRange(attr);
			}
			else
			{
				mapper->InitRange(attr, i);
			}

			FloatArray::Pointer colors = mapper->MapScalars(attr, i);
			if (colors == nullptr)
			{
				return;
			}

			FloatArray::Pointer newPositions = FloatArray::New();
			FloatArray::Pointer newColors = FloatArray::New();
			newPositions->SetElementSize(3);
			newColors->SetElementSize(3);

			float color[3]{};
			for (int i = 0; i < this->GetNumberOfFaces(); i++)
			{
				Face* face = this->GetFace(i);
				auto& p0 = face->Points->GetPoint(0);
				newPositions->AddElement3(p0[0], p0[1], p0[2]);

				auto& p1 = face->Points->GetPoint(1);
				newPositions->AddElement3(p1[0], p1[1], p1[2]);

				auto& p2 = face->Points->GetPoint(2);
				newPositions->AddElement3(p2[0], p2[1], p2[2]);

				colors->GetElement(i, color);
				newColors->AddElement3(color[0], color[1], color[2]);
				newColors->AddElement3(color[0], color[1], color[2]);
				newColors->AddElement3(color[0], color[1], color[2]);
			}

			GLAllocateGLBuffer(m_CellPositionVBO,
				newPositions->GetNumberOfValues() * sizeof(float),
				newPositions->RawPointer());
			GLAllocateGLBuffer(m_CellColorVBO,
				newColors->GetNumberOfValues() * sizeof(float),
				newColors->RawPointer());

			m_CellVAO.vertexBuffer(GL_VBO_IDX_0, m_CellPositionVBO, 0, 3 * sizeof(float));
			GLSetVertexAttrib(m_CellVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
				GL_FALSE, 0);
			m_CellVAO.vertexBuffer(GL_VBO_IDX_1, m_CellColorVBO, 0, 3 * sizeof(float));
			GLSetVertexAttrib(m_CellVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
				GL_FALSE, 0);
		}
	}

private:
	GLVertexArray m_PointVAO, m_LineVAO, m_TriangleVAO;
	GLBuffer m_PositionVBO, m_ColorVBO, m_NormalVBO, m_TextureVBO;
	GLBuffer m_PointEBO, m_LineEBO, m_TriangleEBO;

	GLVertexArray m_CellVAO;
	GLBuffer m_CellPositionVBO, m_CellColorVBO;

	FloatArray::Pointer m_Positions{};
	FloatArray::Pointer m_Colors{};
	IdArray::Pointer m_PointIndices{};
	IdArray::Pointer m_LineIndices{};
	IdArray::Pointer m_TriangleIndices{};

	bool m_Flag{ false };
	bool m_UseColor{ false };
	bool m_ColorWithCell{ false };
	int m_PointSize{ 4 };
	int m_LineWidth{ 1 };

	ArrayObject::Pointer m_ViewAttribute{};
	int m_ViewDemension{};
};
IGAME_NAMESPACE_END
#endif