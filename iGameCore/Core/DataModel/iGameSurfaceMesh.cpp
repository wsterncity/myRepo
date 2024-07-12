#include "iGameSurfaceMesh.h"
#include "iGameScene.h"

IGAME_NAMESPACE_BEGIN

igIndex SurfaceMesh::GetNumberOfEdges()
{
	return m_Edges ? m_Edges->GetNumberOfCells() : 0;
}
igIndex SurfaceMesh::GetNumberOfFaces()
{
	return m_Faces ? m_Faces->GetNumberOfCells() : 0;
}

CellArray* SurfaceMesh::GetEdges()
{
	return m_Edges ? m_Edges.get() : nullptr;
}
CellArray* SurfaceMesh::GetFaces()
{
	return m_Faces ? m_Faces.get() : nullptr;
}

void SurfaceMesh::SetFaces(CellArray::Pointer faces)
{
	if (m_Faces != faces)
	{
		m_Faces = faces;
		this->Modified();
	}
}

Line* SurfaceMesh::GetEdge(igIndex edgeId)
{
	igIndex ncells;
	const igIndex* cell;
	m_Edges->GetCellAtId(edgeId, ncells, cell);

	m_Edge->PointIds->Reset();
	m_Edge->PointIds->InsertNextId(cell[0]);
	m_Edge->PointIds->InsertNextId(cell[1]);
	m_Edge->Points->Reset();
	m_Edge->Points->AddPoint(this->GetPoint(cell[0]));
	m_Edge->Points->AddPoint(this->GetPoint(cell[1]));
	return m_Edge.get();
}

Face* SurfaceMesh::GetFace(igIndex faceId)
{
	igIndex ncells;
	const igIndex* cell;
	m_Faces->GetCellAtId(faceId, ncells, cell);

	Face* face = nullptr;
	if (ncells == 3)
	{
		if (m_Triangle == nullptr)
		{
			m_Triangle = Triangle::New();
		}
		face = m_Triangle.get();
		assert(ncells == 3);
	}
	else if (ncells == 4)
	{
		if (!m_Quad)
		{
			m_Quad = Quad::New();
		}
		face = m_Quad.get();
		assert(ncells == 4);
	}
	else
	{
		if (!m_Polygon)
		{
			m_Polygon = Polygon::New();
		}
		face = m_Polygon.get();
		assert(ncells > 4);
	}

	face->PointIds->Reset();
	face->Points->Reset();

	for (int i = 0; i < ncells; i++) {
		face->PointIds->InsertNextId(cell[i]);
		face->Points->AddPoint(this->GetPoint(cell[i]));
	}

	return face;
}

int SurfaceMesh::GetEdgePointIds(igIndex edgeId, igIndex* ptIds)
{
	if (m_Edges == nullptr)
	{
		this->BuildEdges();
	}
	m_Edges->GetCellAtId(edgeId, ptIds);
	return 2;
}

int SurfaceMesh::GetFacePointIds(igIndex faceId, igIndex* ptIds)
{
	return m_Faces->GetCellAtId(faceId, ptIds);
}

int SurfaceMesh::GetFaceEdgeIds(igIndex faceId, igIndex* edgeIds)
{
	return m_FaceEdges->GetCellAtId(faceId, edgeIds);
}

void SurfaceMesh::BuildEdges()
{
	igIndex i, ncell;
	EdgeTable::Pointer EdgeTable = EdgeTable::New();
	igIndex nfaces = this->GetNumberOfFaces();
	igIndex edgeIds[16]{}, face[32]{};

	m_FaceEdges = CellArray::New();
	EdgeTable->Initialize(this->GetNumberOfPoints());

	for (i = 0; i < nfaces; i++)
	{
		ncell = this->GetFacePointIds(i, face);
		for (int j = 0; j < ncell; j++)
		{
			igIndex idx;
			if ((idx = EdgeTable->IsEdge(face[j], face[(j + 1) % ncell])) == -1)
			{
				idx = EdgeTable->GetNumberOfEdges();
				EdgeTable->InsertEdge(face[j], face[(j + 1) % ncell]);
			}
			edgeIds[j] = idx;
		}
		m_FaceEdges->InsertNextCell(edgeIds, ncell);
	}
	m_Edges = EdgeTable->GetOutput();
}

void SurfaceMesh::BuildEdgeLinks()
{
	if (m_EdgeLinks && m_EdgeLinks->GetMTime() > m_Edge->GetMTime())
	{
		return;
	}

	m_EdgeLinks = CellLinks::New();
	igIndex npts = this->GetNumberOfPoints();
	igIndex nedges = this->GetNumberOfEdges();
	igIndex i, ncell;
	igIndex edge[2]{};


	m_EdgeLinks->Allocate(npts);
	for (i = 0; i < nedges; i++) {
		ncell = m_Edges->GetCellAtId(i, edge);
		m_EdgeLinks->IncrementLinkCount(edge[0]);
		m_EdgeLinks->IncrementLinkCount(edge[1]);
	}

	m_EdgeLinks->AllocateLinks(npts);

	for (i = 0; i < nedges; i++) {
		ncell = m_Edges->GetCellAtId(i, edge);
		m_EdgeLinks->AddCellReference(i, edge[0]);
		m_EdgeLinks->AddCellReference(i, edge[1]);
	}
}

void SurfaceMesh::BuildFaceLinks()
{
	if (m_FaceLinks && m_FaceLinks->GetMTime() > m_Faces->GetMTime())
	{
		return;
	}

	m_FaceLinks = CellLinks::New();
	igIndex npts = this->GetNumberOfPoints();
	igIndex nfaces = this->GetNumberOfFaces();
	igIndex i, ncell;
	igIndex face[32]{};

	m_FaceLinks->Allocate(npts);
	for (i = 0; i < nfaces; i++) {
		ncell = m_Faces->GetCellAtId(i, face);
		for (int j = 0; j < ncell; j++) {
			m_FaceLinks->IncrementLinkCount(face[j]);
		}
	}

	m_FaceLinks->AllocateLinks(npts);
	for (igIndex i = 0; i < nfaces; i++) {
		ncell = m_Faces->GetCellAtId(i, face);
		for (int j = 0; j < ncell; j++) {
			m_FaceLinks->AddCellReference(i, face[j]);
		}
	}
}

void SurfaceMesh::BuildFaceEdgeLinks()
{
	if (m_FaceEdgeLinks && m_FaceEdgeLinks->GetMTime() > m_FaceEdges->GetMTime())
	{
		return;
	}

	m_FaceEdgeLinks = CellLinks::New();
	igIndex nedges = this->GetNumberOfEdges();
	igIndex nfaces = this->GetNumberOfFaces();
	igIndex i, ncell;
	igIndex face[32]{};

	m_FaceEdgeLinks->Allocate(nedges);
	for (igIndex i = 0; i < nfaces; i++) {
		ncell = m_Faces->GetCellAtId(i, face);
		for (int j = 0; j < ncell; j++) {
			m_FaceEdgeLinks->IncrementLinkCount(face[j]);
		}
	}

	m_FaceEdgeLinks->AllocateLinks(nedges);
	for (igIndex i = 0; i < nfaces; i++) {
		ncell = m_Faces->GetCellAtId(i, face);
		for (int j = 0; j < ncell; j++) {
			m_FaceEdgeLinks->AddCellReference(i, face[j]);
		}
	}
}

void SurfaceMesh::Draw(Scene* scene)
{
	if (!m_Visibility)
	{
		return;
	}

	//update uniform buffer
	if (m_UseColor) {
		scene->UBO().useColor = true;
	}
	else {
		scene->UBO().useColor = false;
	}
	scene->UpdateUniformBuffer();

	if (m_UseColor && m_ColorWithCell)
	{
		scene->GetShader(Scene::PATCH)->use();
		m_CellVAO.bind();
		int a = this->GetNumberOfFaces();
		glad_glDrawArrays(GL_TRIANGLES, 0, this->GetNumberOfFaces() * 3);
		m_CellVAO.release();
		return;
	}

	if (m_ViewStyle == IG_POINTS)
	{
		scene->GetShader(Scene::NOLIGHT)->use();
		m_PointVAO.bind();
		glPointSize(m_PointSize);
		auto size = m_Positions->GetNumberOfValues();
		glad_glDrawArrays(GL_POINTS, 0, size / 3);
		m_PointVAO.release();
	}
	else if (m_ViewStyle == IG_WIREFRAME)
	{
		if (m_UseColor)
		{
			scene->GetShader(Scene::NOLIGHT)->use();
		}
		else
		{
			auto shader = scene->GetShader(Scene::PURECOLOR);
			shader->use();
			shader->setUniform(shader->getUniformLocation("inputColor"), igm::vec3{ 0.0f, 0.0f, 0.0f });
		}

		m_LineVAO.bind();
		glLineWidth(m_LineWidth);
		glad_glDrawElements(GL_LINES, m_LineIndices->GetNumberOfValues(), GL_UNSIGNED_INT, 0);
		m_LineVAO.release();
	}
	else if (m_ViewStyle == IG_SURFACE)
	{
		scene->GetShader(Scene::PATCH)->use();
		m_TriangleVAO.bind();
		glad_glDrawElements(GL_TRIANGLES, m_TriangleIndices->GetNumberOfValues(), GL_UNSIGNED_INT, 0);
		m_TriangleVAO.release();
	}
	else if (m_ViewStyle == IG_SURFACE_WITH_EDGE)
	{
		if (m_UseColor)
		{
			scene->GetShader(Scene::NOLIGHT)->use();
		}
		else
		{
			auto shader = scene->GetShader(Scene::PURECOLOR);
			shader->use();
			shader->setUniform(shader->getUniformLocation("inputColor"), igm::vec3{ 0.0f, 0.0f, 0.0f });
		}

		m_LineVAO.bind();
		glLineWidth(m_LineWidth);
		glad_glDrawElements(GL_LINES, m_LineIndices->GetNumberOfValues(), GL_UNSIGNED_INT, 0);
		m_LineVAO.release();

		scene->GetShader(Scene::PATCH)->use();
		m_TriangleVAO.bind();
		glad_glDrawElements(GL_TRIANGLES, m_TriangleIndices->GetNumberOfValues(), GL_UNSIGNED_INT, 0);
		m_TriangleVAO.release();
	}
}

void SurfaceMesh::ConvertToDrawableData()
{
	if (m_Positions && m_Positions->GetMTime() > this->GetMTime())
	{
		return;
	}

	if (!m_Flag)
	{
		m_PointVAO.create();
		m_LineVAO.create();
		m_TriangleVAO.create();
		m_PositionVBO.create();
		m_ColorVBO.create();
		m_NormalVBO.create();
		m_TextureVBO.create();
		m_PointEBO.create();
		m_LineEBO.create();
		m_TriangleEBO.create();

		m_CellVAO.create();
		m_CellPositionVBO.create();
		m_CellColorVBO.create();
		m_Flag = true;
	}

	m_Positions = m_Points->ConvertToDataArray();
	m_Positions->Modified();

	//m_PointVAO.destroy();
	//m_LineVAO.destroy();
	//m_TriangleVAO.destroy();
	//m_PositionVBO.destroy();
	//m_ColorVBO.destroy();
	//m_NormalVBO.destroy();
	//m_TextureVBO.destroy();
	//m_PointEBO.destroy();
	//m_LineEBO.destroy();
	//m_TriangleEBO.destroy();

	// set line indices
	if (this->GetEdges() == nullptr)
	{
		this->BuildEdges();
	}
	m_LineIndices = this->GetEdges()->ConvertToDataArray();

	// set triangle indices
	IndexArray::Pointer triangleIndices = IndexArray::New();
	triangleIndices->SetNumberOfComponents(3);
	igIndex i, ncell;
	igIndex cell[32]{};
	for (i = 0; i < this->GetNumberOfFaces(); i++) {
		ncell = this->GetFacePointIds(i, cell);
		igIndex v0 = cell[0];
		for (int j = 2; j < ncell; j++)
		{
			triangleIndices->InsertNextTuple3(v0, cell[j - 1], cell[j]);
		}
	}
	m_TriangleIndices = triangleIndices;


	m_PointVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
	GLSetVertexAttrib(m_PointVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
		GL_FALSE, 0);

	// line
	m_LineVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
	GLSetVertexAttrib(m_LineVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
		GL_FALSE, 0);
	m_LineVAO.elementBuffer(m_LineEBO);

	// triangle
	m_TriangleVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
	GLSetVertexAttrib(m_TriangleVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
		GL_FALSE, 0);
	m_TriangleVAO.elementBuffer(m_TriangleEBO);


	GLAllocateGLBuffer(m_PositionVBO, 
					   m_Positions->GetNumberOfValues() * sizeof(float), 
					   m_Positions->GetRawPointer());

	GLAllocateGLBuffer(m_LineEBO,
					   m_LineIndices->GetNumberOfValues() * sizeof(igIndex),
					   m_LineIndices->GetRawPointer());

	GLAllocateGLBuffer(m_TriangleEBO,
					   m_TriangleIndices->GetNumberOfValues() * sizeof(igIndex),
					   m_TriangleIndices->GetRawPointer());
}
IGAME_NAMESPACE_END