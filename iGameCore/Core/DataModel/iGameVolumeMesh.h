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

	igIndex GetNumberOfVolumes();
	
	CellArray* GetVolumes();

	void SetVolumes(CellArray::Pointer volumes);

	Volume* GetVolume(igIndex volumeId);

	int GetVolumePointIds(igIndex volumeId, igIndex* ptIds);

	void BuildFaces();

	void BuildVolumeLinks();

	void BuildVolumeEdgeLinks();

	void BuildVolumeFaceLinks();


protected:
	VolumeMesh() {};
	~VolumeMesh() override {};

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
		auto& attr = this->GetAttributes()->GetAttribute(index);
		if (attr.active)
		{
			if (attr.attachmentType == IG_POINT)
				this->SetAttributeWithPointData(attr.array, demension);
			else if (attr.attachmentType == IG_CELL)
				this->SetAttributeWithCellData(attr.array, demension);
		}
	}

	void SetAttributeWithPointData(DataArray::Pointer attr, igIndex i = -1) override
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
				m_Colors->GetRawPointer());

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

	void SetAttributeWithCellData(DataArray::Pointer attr, igIndex i = -1)
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
			newPositions->SetNumberOfComponents(3);
			newColors->SetNumberOfComponents(3);

			float color[3]{};
			for (int i = 0; i < this->GetNumberOfFaces(); i++)
			{
				Face* face = this->GetFace(i);
				auto& p0 = face->Points->GetPoint(0);
				newPositions->InsertNextTuple3(p0[0], p0[1], p0[2]);

				auto& p1 = face->Points->GetPoint(1);
				newPositions->InsertNextTuple3(p1[0], p1[1], p1[2]);

				auto& p2 = face->Points->GetPoint(2);
				newPositions->InsertNextTuple3(p2[0], p2[1], p2[2]);

				colors->GetTuple(i, color);
				newColors->InsertNextTuple3(color[0], color[1], color[2]);
				newColors->InsertNextTuple3(color[0], color[1], color[2]);
				newColors->InsertNextTuple3(color[0], color[1], color[2]);
			}

			GLAllocateGLBuffer(m_CellPositionVBO,
				newPositions->GetNumberOfValues() * sizeof(float),
				newPositions->GetRawPointer());
			GLAllocateGLBuffer(m_CellColorVBO,
				newColors->GetNumberOfValues() * sizeof(float),
				newColors->GetRawPointer());

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
	IndexArray::Pointer m_PointIndices{};
	IndexArray::Pointer m_LineIndices{};
	IndexArray::Pointer m_TriangleIndices{};

	bool m_Flag{ false };
	bool m_UseColor{ false };
	bool m_ColorWithCell{ false };
	int m_PointSize{ 4 };
	int m_LineWidth{ 1 };

	DataArray::Pointer m_ViewAttribute;
	int m_ViewDemension;
};
IGAME_NAMESPACE_END
#endif