#include "iGameVolumeMesh.h"
#include "iGameScene.h"
#include "iGameFaceTable.h"


IGAME_NAMESPACE_BEGIN

igIndex VolumeMesh::GetNumberOfVolumes()
{
	return m_Volumes ? m_Volumes->GetNumberOfCells() : 0;
}

CellArray* VolumeMesh::GetVolumes()
{
	return m_Volumes ? m_Volumes.get() : nullptr;
}

void VolumeMesh::SetVolumes(CellArray::Pointer volumes)
{
	if (m_Volumes != volumes)
	{
		m_Volumes = volumes;
		this->Modified();
	}
}

Volume* VolumeMesh::GetVolume(igIndex volumeId)
{
	igIndex ncells;
	const igIndex* cell;
	m_Volumes->GetCellAtId(volumeId, ncells, cell);

	Volume* volume = nullptr;
	if (ncells == Tetra::NumberOfPoints)
	{
		if (m_Tetra == nullptr)
		{
			m_Tetra = Tetra::New();
		}
		volume = m_Tetra.get();
	}
	else if (ncells == Hexahedron::NumberOfPoints)
	{
		if (m_Hexahedron == nullptr)
		{
			m_Hexahedron = Hexahedron::New();
		}
		volume = m_Hexahedron.get();
	}
	else if (ncells == Prism::NumberOfPoints)
	{
		if (m_Prism == nullptr)
		{
			m_Prism = Prism::New();
		}
		volume = m_Prism.get();
	}
	else if (ncells == Pyramid::NumberOfPoints)
	{
		if (m_Pyramid == nullptr)
		{
			m_Pyramid = Pyramid::New();
		}
		volume = m_Pyramid.get();
	}
	assert(volume != nullptr);
	volume->PointIds->Reset();
	volume->Points->Reset();

	for (int i = 0; i < ncells; i++) {
		volume->PointIds->InsertNextId(cell[0]);
		volume->Points->AddPoint(this->GetPoint(cell[0]));
	}

	return volume;
}

int VolumeMesh::GetVolumePointIds(igIndex volumeId, igIndex* ptIds)
{
	return m_Volumes->GetCellAtId(volumeId, ptIds);
}


void VolumeMesh::BuildFaces()
{
	igIndex i, j, k;
	igIndex ncell;

	FaceTable::Pointer FaceTable = FaceTable::New();
	igIndex cell[64]{}, faceIds[64]{}, face[64]{};

	m_VolumeFaces = CellArray::New();
	for (i = 0; i < this->GetNumberOfVolumes(); i++)
	{
		Volume* vol = this->GetVolume(i);
		ncell = m_Volumes->GetCellAtId(i, cell);
		for (j = 0; j < vol->GetNumberOfFaces(); j++) // number of faces
		{
			const igIndex* index;
			int size = vol->GetFacePoints(j, index); // this face's number of vertices 
			for (k = 0; k < size; k++) {
				face[k] = cell[index[k]];
			}
			igIndex idx;
			if ((idx = FaceTable->IsFace(face, size)) == -1) {
				idx = FaceTable->GetNumberOfFaces();
				FaceTable->InsertFace(face, size);
			}
			faceIds[j] = idx;
		}
		m_VolumeFaces->InsertNextCell(faceIds, vol->GetNumberOfFaces());
	}

	m_Faces = FaceTable->GetOutput();
}

void VolumeMesh::BuildVolumeLinks()
{
	if (m_VolumeLinks && m_VolumeLinks->GetMTime() > m_Volumes->GetMTime())
	{
		return;
	}

	m_VolumeLinks = CellLinks::New();
	igIndex npts = this->GetNumberOfPoints();
	igIndex nvolumes = this->GetNumberOfVolumes();
	igIndex i, ncell;
	igIndex cell[32]{};


	m_VolumeLinks->Allocate(npts);
	for (i = 0; i < nvolumes; i++) {
		ncell = m_Volumes->GetCellAtId(i, cell);
		for (int j = 0; j < ncell; j++) {
			m_VolumeLinks->IncrementLinkCount(cell[j]);
		}
	}

	m_VolumeLinks->AllocateLinks(npts);
	for (i = 0; i < nvolumes; i++) {
		ncell = m_Volumes->GetCellAtId(i, cell);
		for (int j = 0; j < ncell; j++) {
			m_VolumeLinks->AddCellReference(i, cell[j]);
		}
	}
}

void VolumeMesh::BuildVolumeEdgeLinks()
{
	if (m_VolumeEdgeLinks && m_VolumeEdgeLinks->GetMTime() > m_VolumeEdges->GetMTime())
	{
		return;
	}

	m_VolumeEdgeLinks = CellLinks::New();
	igIndex nedges = this->GetNumberOfEdges();
	igIndex nvolumes = this->GetNumberOfVolumes();
	igIndex i, ncell;
	igIndex cell[32]{};

	m_VolumeEdgeLinks->Allocate(nedges);
	for (i = 0; i < nvolumes; i++) {
		ncell = m_VolumeEdges->GetCellAtId(i, cell);
		for (int j = 0; j < ncell; j++) {
			m_VolumeEdgeLinks->IncrementLinkCount(cell[j]);
		}
	}

	m_VolumeEdgeLinks->AllocateLinks(nedges);
	for (i = 0; i < nvolumes; i++) {
		ncell = m_VolumeEdges->GetCellAtId(i, cell);
		for (int j = 0; j < ncell; j++) {
			m_VolumeEdgeLinks->AddCellReference(i, cell[j]);
		}
	}
}

void VolumeMesh::BuildVolumeFaceLinks()
{
	if (m_VolumeFaceLinks && m_VolumeFaceLinks->GetMTime() > m_VolumeFaces->GetMTime())
	{
		return;
	}

	m_VolumeFaceLinks = CellLinks::New();
	igIndex nfaces = this->GetNumberOfFaces();
	igIndex nvolumes = this->GetNumberOfVolumes();
	igIndex i, ncell;
	igIndex cell[32]{};

	m_VolumeFaceLinks->Allocate(nfaces);
	for (i = 0; i < nvolumes; i++) {
		ncell = m_VolumeFaces->GetCellAtId(i, cell);
		for (int j = 0; j < ncell; j++) {
			m_VolumeFaceLinks->IncrementLinkCount(cell[j]);
		}
	}

	m_VolumeFaceLinks->AllocateLinks(nfaces);
	for (i = 0; i < nvolumes; i++) {
		ncell = m_VolumeFaces->GetCellAtId(i, cell);
		for (int j = 0; j < ncell; j++) {
			m_VolumeFaceLinks->AddCellReference(i, cell[j]);
		}
	}
}

void VolumeMesh::Draw(Scene* scene)
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

void VolumeMesh::ConvertToDrawableData()
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

	if (this->GetFaces() == nullptr)
	{
		this->BuildFaces();
	}

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
	//m_LineVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
	//GLSetVertexAttrib(m_LineVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
	//	GL_FALSE, 0);
	//m_LineVAO.elementBuffer(m_LineEBO);

	// triangle
	m_TriangleVAO.vertexBuffer(GL_VBO_IDX_0, m_PositionVBO, 0, 3 * sizeof(float));
	GLSetVertexAttrib(m_TriangleVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
		GL_FALSE, 0);
	m_TriangleVAO.elementBuffer(m_TriangleEBO);


	GLAllocateGLBuffer(m_PositionVBO,
		m_Positions->GetNumberOfValues() * sizeof(float),
		m_Positions->GetRawPointer());

	//GLAllocateGLBuffer(m_LineEBO,
	//	m_LineIndices->GetNumberOfValues() * sizeof(igIndex),
	//	m_LineIndices->GetRawPointer());

	GLAllocateGLBuffer(m_TriangleEBO,
		m_TriangleIndices->GetNumberOfValues() * sizeof(igIndex),
		m_TriangleIndices->GetRawPointer());
}
IGAME_NAMESPACE_END