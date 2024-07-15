#ifndef iGameSurfaceMesh_h
#define iGameSurfaceMesh_h

#include "iGamePointSet.h"
#include "iGameCellArray.h"
#include "iGameEdgeTable.h"
#include "iGameCellLinks.h"

#include "iGameLine.h"
#include "iGameTriangle.h"
#include "iGameQuad.h"
#include "iGamePolygon.h"


IGAME_NAMESPACE_BEGIN
class SurfaceMesh : public PointSet {
public:
	I_OBJECT(SurfaceMesh);
	static Pointer New() { return new SurfaceMesh; }

	igIndex GetNumberOfEdges();
	igIndex GetNumberOfFaces();
	
	CellArray* GetEdges();
	CellArray* GetFaces();

	void SetFaces(CellArray::Pointer faces);

	Line* GetEdge(igIndex edgeId);

	Face* GetFace(igIndex faceId);

	int GetEdgePointIds(igIndex edgeId, igIndex* ptIds);

	int GetFacePointIds(igIndex faceId, igIndex* ptIds);

	int GetFaceEdgeIds(igIndex faceId, igIndex* edgeIds);

	void BuildEdges();

	void BuildEdgeLinks();

	void BuildFaceLinks();

	void BuildFaceEdgeLinks();

protected:
	SurfaceMesh() 
	{
		m_ViewStyle = IG_SURFACE;
	};
	~SurfaceMesh() override {};

	CellArray::Pointer m_Edges{};        // The point set of edges
	CellLinks::Pointer m_EdgeLinks{};    // The adjacent edges of points

	CellArray::Pointer m_Faces{};        // The point set of faces
	CellLinks::Pointer m_FaceLinks{};    // The adjacent faces of points

	CellArray::Pointer m_FaceEdges{};    // The edge set of faces
	CellLinks::Pointer m_FaceEdgeLinks{};// The adjacent faces of edges

private:
	Line::Pointer m_Edge{};            // Used for the returned 'Line' object, which is Thread-Unsafe
	Triangle::Pointer m_Triangle{};    // Used for the returned 'Triangle' object, which is Thread-Unsafe
	Quad::Pointer m_Quad{};            // Used for the returned 'Quad' object, which is Thread-Unsafe
	Polygon::Pointer m_Polygon{};      // Used for the returned 'Polygon' object, which is Thread-Unsafe
	
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
			if(attr.attachmentType == IG_POINT)
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
				for (int j = 2; j < face->GetCellSize(); j++) {
					auto& p0 = face->Points->GetPoint(0);
					newPositions->InsertNextTuple3(p0[0], p0[1], p0[2]);

					auto& p1 = face->Points->GetPoint(j - 1);
					newPositions->InsertNextTuple3(p1[0], p1[1], p1[2]);

					auto& p2 = face->Points->GetPoint(j);
					newPositions->InsertNextTuple3(p2[0], p2[1], p2[2]);

					colors->GetTuple(i, color);
					newColors->InsertNextTuple3(color[0], color[1], color[2]);
					newColors->InsertNextTuple3(color[0], color[1], color[2]);
					newColors->InsertNextTuple3(color[0], color[1], color[2]);
				}
			}
			m_CellPositionSize = newPositions->GetNumberOfTuples();

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
	int m_CellPositionSize{};

	FloatArray::Pointer m_Positions{};
	FloatArray::Pointer m_Colors{};
	IndexArray::Pointer m_PointIndices{};
	IndexArray::Pointer m_LineIndices{};
	IndexArray::Pointer m_TriangleIndices{};

	bool m_Flag{ false };
	bool m_UseColor{ false };
	bool m_ColorWithCell{ false };
	int m_PointSize{ 1 };
	int m_LineWidth{ 1 };

	DataArray::Pointer m_ViewAttribute;
	int m_ViewDemension;
};
IGAME_NAMESPACE_END
#endif