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

Line* SurfaceMesh::GetEdge(const IGsize edgeId)
{
	const igIndex* cell;
	int ncells = m_Edges->GetCellIds(edgeId, cell);

	m_Edge->PointIds->Reset();
	m_Edge->PointIds->AddId(cell[0]);
	m_Edge->PointIds->AddId(cell[1]);
	m_Edge->Points->Reset();
	m_Edge->Points->AddPoint(this->GetPoint(cell[0]));
	m_Edge->Points->AddPoint(this->GetPoint(cell[1]));
	return m_Edge.get();
}

Face* SurfaceMesh::GetFace(const IGsize faceId)
{
	const igIndex* cell;
	int ncells = m_Faces->GetCellIds(faceId, cell);

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
		face->PointIds->AddId(cell[i]);
		face->Points->AddPoint(this->GetPoint(cell[i]));
	}

	return face;
}

int SurfaceMesh::GetEdgePointIds(const IGsize edgeId, igIndex* ptIds)
{
	if (m_Edges == nullptr)
	{
		this->BuildEdges();
	}
	m_Edges->GetCellIds(edgeId, ptIds);
	return 2;
}

int SurfaceMesh::GetFacePointIds(const IGsize faceId, igIndex* ptIds)
{
	return m_Faces->GetCellIds(faceId, ptIds);
}

int SurfaceMesh::GetFaceEdgeIds(const IGsize faceId, igIndex* edgeIds)
{
	return m_FaceEdges->GetCellIds(faceId, edgeIds);
}

void SurfaceMesh::BuildEdges()
{
	igIndex i, ncell;
	EdgeTable::Pointer EdgeTable = EdgeTable::New();
	igIndex nfaces = this->GetNumberOfFaces();
	igIndex edgeIds[32]{}, face[32]{};

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
		m_FaceEdges->AddCellIds(edgeIds, ncell);
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
	IGsize npts = GetNumberOfPoints();
	IGsize nedges = GetNumberOfEdges();
	igIndex e[2]{};

	m_EdgeLinks->Allocate(npts);
	for (int i = 0; i < nedges; i++) {
		int size = m_Edges->GetCellIds(i, e);
		m_EdgeLinks->IncrementLinkSize(e[0]);
		m_EdgeLinks->IncrementLinkSize(e[1]);
	}

	m_EdgeLinks->AllocateLinks(npts);

	for (int i = 0; i < nedges; i++) {
		int size = m_Edges->GetCellIds(i, e);
		m_EdgeLinks->AddReference(e[0], i);
		m_EdgeLinks->AddReference(e[1], i);
	}
	m_EdgeLinks->Modified();
}

void SurfaceMesh::BuildFaceLinks()
{
	if (m_FaceLinks && m_FaceLinks->GetMTime() > m_Faces->GetMTime())
	{
		return;
	}

	m_FaceLinks = CellLinks::New();
	IGsize npts = GetNumberOfPoints();
	IGsize nfaces = GetNumberOfFaces();
	igIndex face[32]{};

	m_FaceLinks->Allocate(npts);
	for (int i = 0; i < nfaces; i++) {
		int size = m_Faces->GetCellIds(i, face);
		for (int j = 0; j < size; j++) {
			m_FaceLinks->IncrementLinkSize(face[j]);
		}
	}

	m_FaceLinks->AllocateLinks(npts);
	for (int i = 0; i < nfaces; i++) {
		int size = m_Faces->GetCellIds(i, face);
		for (int j = 0; j < size; j++) {
			m_FaceLinks->AddReference(face[j], i);
		}
	}
	m_FaceLinks->Modified();
}

void SurfaceMesh::BuildFaceEdgeLinks()
{
	if (m_FaceEdgeLinks && m_FaceEdgeLinks->GetMTime() > m_FaceEdges->GetMTime())
	{
		return;
	}

	m_FaceEdgeLinks = CellLinks::New();
	IGsize nedges = GetNumberOfEdges();
	IGsize nfaces = GetNumberOfFaces();
	igIndex face[32]{};

	m_FaceEdgeLinks->Allocate(nedges);
	for (int i = 0; i < nfaces; i++) {
		int size = m_FaceEdges->GetCellIds(i, face);
		for (int j = 0; j < size; j++) {
			m_FaceEdgeLinks->IncrementLinkSize(face[j]);
		}
	}

	m_FaceEdgeLinks->AllocateLinks(nedges);
	for (int i = 0; i < nfaces; i++) {
		int size = m_FaceEdges->GetCellIds(i, face);
		for (int j = 0; j < size; j++) {
			m_FaceEdgeLinks->AddReference(face[j], i);
		}
	}
}

int SurfaceMesh::GetPointToOneRingPoints(const IGsize ptId, igIndex* ptIds) {
	assert(ptId < GetNumberOfPoints() && "ptId too large");
	auto& link = m_EdgeLinks->GetLink(ptId);
	igIndex e[2]{};
	for (int i = 0; i < link.size; i++) {
		GetEdgePointIds(link.pointer[i], e);
		ptIds[i] = e[0] - ptId + e[1];
		assert(e[0] == ptId || e[1] == ptId);
	}
	return link.size;
}
int SurfaceMesh::GetPointToNeighborEdges(const IGsize ptId, igIndex* edgeIds) {
	assert(ptId < GetNumberOfPoints() && "ptId too large");
	auto& link = m_EdgeLinks->GetLink(ptId);
	for (int i = 0; i < link.size; i++) {
		edgeIds[i] = link.pointer[i];
	}
	return link.size;
}
int SurfaceMesh::GetPointToNeighborFaces(const IGsize ptId, igIndex* faceIds) {
	assert(ptId < GetNumberOfPoints() && "ptId too large");
	auto& link = m_FaceLinks->GetLink(ptId);
	for (int i = 0; i < link.size; i++) {
		faceIds[i] = link.pointer[i];
	}
	return link.size;
}
int SurfaceMesh::GetEdgeToNeighborFaces(const IGsize edgeId, igIndex* faceIds) {
	assert(edgeId < GetNumberOfEdges() && "edgeId too large");
	auto& link = m_FaceEdgeLinks->GetLink(edgeId);
	for (int i = 0; i < link.size; i++) {
		faceIds[i] = link.pointer[i];
	}
	return link.size;
}
int SurfaceMesh::GetEdgeToOneRingFaces(const IGsize edgeId, igIndex* faceIds) {
	assert(edgeId < GetNumberOfEdges() && "edgeId too large");
	igIndex e[2]{};
	GetEdgePointIds(edgeId, e);
	std::set<igIndex> st;
	for (int i = 0; i < 2; i++) {
		auto& link = m_FaceLinks->GetLink(e[i]);
		for (int j = 0; j < link.size; j++) {
			st.insert(link.pointer[j]);
		}
	}
	int i = 0;
	for (auto& fid : st) {
		faceIds[i++] = fid;
	}
	return i;
}
int SurfaceMesh::GetFaceToNeighborFaces(const IGsize faceId, igIndex* faceIds) {
	assert(faceId < GetNumberOfFaces() && "faceId too large");
	igIndex edgeIds[32]{};
	int size = GetFaceEdgeIds(faceId, edgeIds);
	std::set<igIndex> st;
	for (int i = 0; i < size; i++) {
		auto& link = m_FaceEdgeLinks->GetLink(edgeIds[i]);
		for (int j = 0; j < link.size; j++) {
			st.insert(link.pointer[j]);
		}
	}
	int i = 0;
	for (auto& fid : st) {
		if (fid != faceId) {
			faceIds[i++] = fid;
		}
	}
	return i;
}
int SurfaceMesh::GetFaceToOneRingFaces(const IGsize faceId, igIndex* faceIds) {
	assert(faceId < GetNumberOfFaces() && "faceId too large");
	igIndex ptIds[32]{};
	int size = GetFacePointIds(faceId, ptIds);
	std::set<igIndex> st;
	for (int i = 0; i < size; i++) {
		auto& link = m_FaceLinks->GetLink(ptIds[i]);
		for (int j = 0; j < link.size; j++) {
			st.insert(link.pointer[j]);
		}
	}
	int i = 0;
	for (auto& fid : st) {
		if (fid != faceId) {
			faceIds[i++] = fid;
		}
	}
	return i;
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
		glad_glDrawArrays(GL_TRIANGLES, 0, m_CellPositionSize);
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
		glad_glDrawElements(GL_LINES, m_LineIndices->GetNumberOfIds(), GL_UNSIGNED_INT, 0);
		m_LineVAO.release();
	}
	else if (m_ViewStyle == IG_SURFACE)
	{
		scene->GetShader(Scene::PATCH)->use();
		m_TriangleVAO.bind();
		glad_glDrawElements(GL_TRIANGLES, m_TriangleIndices->GetNumberOfIds(), GL_UNSIGNED_INT, 0);
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
		glad_glDrawElements(GL_LINES, m_LineIndices->GetNumberOfIds(), GL_UNSIGNED_INT, 0);
		m_LineVAO.release();

		scene->GetShader(Scene::PATCH)->use();
		m_TriangleVAO.bind();
		glad_glDrawElements(GL_TRIANGLES, m_TriangleIndices->GetNumberOfIds(), GL_UNSIGNED_INT, 0);
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

	m_Positions = m_Points->ConvertToArray();
	m_Positions->Modified();

	//std::cout << m_Positions->GetMTime() << "  " << GetMTime() << std::endl;
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
	m_LineIndices = this->GetEdges()->GetCellIdArray();

	// set triangle indices
	IdArray::Pointer triangleIndices = IdArray::New();
	int i, ncell;
	igIndex cell[32]{};
	int a = this->GetNumberOfFaces();
	for (i = 0; i < this->GetNumberOfFaces(); i++) {
		ncell = this->GetFacePointIds(i, cell);
		igIndex v0 = cell[0];
		for (int j = 2; j < ncell; j++)
		{
			triangleIndices->AddId(v0);
			triangleIndices->AddId(cell[j - 1]);
			triangleIndices->AddId(cell[j]);
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
					   m_Positions->RawPointer());

	GLAllocateGLBuffer(m_LineEBO,
					   m_LineIndices->GetNumberOfIds() * sizeof(igIndex),
					   m_LineIndices->RawPointer());

	GLAllocateGLBuffer(m_TriangleEBO,
					   m_TriangleIndices->GetNumberOfIds() * sizeof(igIndex),
					   m_TriangleIndices->RawPointer());
}
IGAME_NAMESPACE_END