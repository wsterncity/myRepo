#include "iGameVolumeMesh.h"
#include "iGameScene.h"
#include "iGameFaceTable.h"


IGAME_NAMESPACE_BEGIN

IGsize VolumeMesh::GetNumberOfVolumes() const noexcept
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

Volume* VolumeMesh::GetVolume(const IGsize volumeId)
{
	const igIndex* cell;
	int ncells = m_Volumes->GetCellIds(volumeId, cell);

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
		volume->PointIds->AddId(cell[0]);
		volume->Points->AddPoint(this->GetPoint(cell[0]));
	}

	return volume;
}

int VolumeMesh::GetVolumePointIds(const IGsize volumeId, igIndex* ptIds)
{
	return m_Volumes->GetCellIds(volumeId, ptIds);
}

int VolumeMesh::GetVolumeEdgeIds(const IGsize volumeId, igIndex* edgeIds)
{
	return m_VolumeEdges->GetCellIds(volumeId, edgeIds);
}

int VolumeMesh::GetVolumeFaceIds(const IGsize volumeId, igIndex* faceIds)
{
	return m_VolumeFaces->GetCellIds(volumeId, faceIds);
}


void VolumeMesh::BuildFaceAndEdges()
{
	FaceTable::Pointer FaceTable = FaceTable::New();
	EdgeTable::Pointer EdgeTable = EdgeTable::New();
	igIndex cell[64]{}, faceIds[64]{}, edgeIds[64]{}, face[64]{}, edge[2]{};

	m_VolumeEdges = CellArray::New();
	m_VolumeFaces = CellArray::New();

	for (IGsize i = 0; i < this->GetNumberOfVolumes(); i++)
	{
		Volume* vol = this->GetVolume(i);
		int size = m_Volumes->GetCellIds(i, cell);
		for (int j = 0; j < vol->GetNumberOfFaces(); j++) // number of faces
		{
			const igIndex* index;
			int size = vol->GetFacePoints(j, index); // this face's number of points 
			for (int k = 0; k < size; k++) {
				face[k] = cell[index[k]];
			}
			igIndex idx;
			if ((idx = FaceTable->IsFace(face, size)) == -1) {
				idx = FaceTable->GetNumberOfFaces();
				FaceTable->InsertFace(face, size);
			}
			faceIds[j] = idx;
		}
		m_VolumeFaces->AddCellIds(faceIds, vol->GetNumberOfFaces());

		for (int j = 0; j < vol->GetNumberOfEdges(); j++)
		{
			const igIndex* index;
			vol->GetEdgePoints(j, index); // this edge's number of points
			for (int k = 0; k < 2; k++) {
				edge[k] = cell[index[k]];
			}
			igIndex idx;
			if ((idx = EdgeTable->IsEdge(edge[0], edge[1])) == -1) {
				idx = EdgeTable->GetNumberOfEdges();
				EdgeTable->InsertEdge(edge[0], edge[1]);
			}
			edgeIds[j] = idx;
		}
		m_VolumeEdges->AddCellIds(edgeIds, vol->GetNumberOfEdges());
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
	IGsize npts = GetNumberOfPoints();
	IGsize nvolumes = GetNumberOfVolumes();
	igIndex cell[32]{};

	m_VolumeLinks->Allocate(npts);
	for (IGsize i = 0; i < nvolumes; i++) {
		int size = m_Volumes->GetCellIds(i, cell);
		for (int j = 0; j < size; j++) {
			m_VolumeLinks->IncrementLinkSize(cell[j]);
		}
	}

	m_VolumeLinks->AllocateLinks(npts);
	for (IGsize i = 0; i < nvolumes; i++) {
		int size = m_Volumes->GetCellIds(i, cell);
		for (int j = 0; j < size; j++) {
			m_VolumeLinks->AddReference(cell[j], i);
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
	IGsize nedges = this->GetNumberOfEdges();
	IGsize nvolumes = this->GetNumberOfVolumes();
	igIndex cell[32]{};

	m_VolumeEdgeLinks->Allocate(nedges);
	for (IGsize i = 0; i < nvolumes; i++) {
		int size = m_VolumeEdges->GetCellIds(i, cell);
		for (int j = 0; j < size; j++) {
			m_VolumeEdgeLinks->IncrementLinkSize(cell[j]);
		}
	}

	m_VolumeEdgeLinks->AllocateLinks(nedges);
	for (IGsize i = 0; i < nvolumes; i++) {
		int size = m_VolumeEdges->GetCellIds(i, cell);
		for (int j = 0; j < size; j++) {
			m_VolumeEdgeLinks->AddReference(cell[j], i);
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
	IGsize nfaces = this->GetNumberOfFaces();
	IGsize nvolumes = this->GetNumberOfVolumes();
	igIndex cell[32]{};

	m_VolumeFaceLinks->Allocate(nfaces);
	for (IGsize i = 0; i < nvolumes; i++) {
		int size = m_VolumeFaces->GetCellIds(i, cell);
		for (int j = 0; j < size; j++) {
			m_VolumeFaceLinks->IncrementLinkSize(cell[j]);
		}
	}

	m_VolumeFaceLinks->AllocateLinks(nfaces);
	for (IGsize i = 0; i < nvolumes; i++) {
		int size = m_VolumeFaces->GetCellIds(i, cell);
		for (int j = 0; j < size; j++) {
			m_VolumeFaceLinks->AddReference(cell[j], i);
		}
	}
}

int VolumeMesh::GetPointToNeighborVolumes(const IGsize ptId, igIndex* volumeIds)
{
	assert(ptId < GetNumberOfPoints() && "ptId too large");
	auto& link = m_VolumeLinks->GetLink(ptId);
	for (int i = 0; i < link.size; i++) {
		volumeIds[i] = link.pointer[i];
	}
	return link.size;
}

int VolumeMesh::GetEdgeToNeighborVolumes(const IGsize edgeId, igIndex* volumeIds)
{
	assert(edgeId < GetNumberOfEdges() && "edgeId too large");
	auto& link = m_VolumeEdgeLinks->GetLink(edgeId);
	for (int i = 0; i < link.size; i++) {
		volumeIds[i] = link.pointer[i];
	}
	return link.size;
}

int VolumeMesh::GetFaceToNeighborVolumes(const IGsize faceId, igIndex* volumeIds)
{
	assert(faceId < GetNumberOfFaces() && "faceId too large");
	auto& link = m_VolumeFaceLinks->GetLink(faceId);
	for (int i = 0; i < link.size; i++) {
		volumeIds[i] = link.pointer[i];
	}
	return link.size;
}

int VolumeMesh::GetVolumeToNeighborVolumesWithPoint(const IGsize vid, igIndex* volumeIds)
{
	assert(vid < GetNumberOfVolumes() && "volumeId too large");
	igIndex ids[32]{};
	int size = GetVolumePointIds(vid, ids);
	std::set<igIndex> st;
	for (int i = 0; i < size; i++) {
		auto& link = m_VolumeLinks->GetLink(ids[i]);
		for (int j = 0; j < link.size; j++) {
			st.insert(link.pointer[j]);
		}
	}
	int i = 0;
	for (auto& _vid : st) {
		if (_vid != vid) {
			volumeIds[i++] = _vid;
		}
	}
	return i;
}

int VolumeMesh::GetVolumeToNeighborVolumesWithEdge(const IGsize vid, igIndex* volumeIds)
{
	assert(vid < GetNumberOfVolumes() && "volumeId too large");
	igIndex ids[32]{};
	int size = GetVolumeEdgeIds(vid, ids);
	std::set<igIndex> st;
	for (int i = 0; i < size; i++) {
		auto& link = m_VolumeEdgeLinks->GetLink(ids[i]);
		for (int j = 0; j < link.size; j++) {
			st.insert(link.pointer[j]);
		}
	}
	int i = 0;
	for (auto& _vid : st) {
		if (_vid != vid) {
			volumeIds[i++] = _vid;
		}
	}
	return i;
}

int VolumeMesh::GetVolumeToNeighborVolumesWithFace(const IGsize vid, igIndex* volumeIds)
{
	assert(vid < GetNumberOfVolumes() && "volumeId too large");
	igIndex ids[32]{};
	int size = GetVolumeFaceIds(vid, ids);
	std::set<igIndex> st;
	for (int i = 0; i < size; i++) {
		auto& link = m_VolumeFaceLinks->GetLink(ids[i]);
		for (int j = 0; j < link.size; j++) {
			st.insert(link.pointer[j]);
		}
	}
	int i = 0;
	for (auto& _vid : st) {
		if (_vid != vid) {
			volumeIds[i++] = _vid;
		}
	}
	return i;
}

igIndex VolumeMesh::GetVolumeIdFormPointIds(igIndex* ids, int size)
{
	IGsize sum = 0;
	for (int i = 0; i < size; i++) {
		if (ids[i] >= this->GetNumberOfPoints()) {
			return (-1);
		}
		sum += ids[i];
	}

	igIndex volumeIds[128]{}, ptIds[32]{};
	int size1 = GetPointToNeighborVolumes(ids[0], volumeIds);
	for (int i = 0; i < size1; i++)
	{
		if (size != GetVolumePointIds(volumeIds[i], ptIds)) continue;
		IGsize index_sum = 0;
		for (int j = 0; j < size; j++)
		{
			index_sum += ptIds[j];
		}
		if (sum == index_sum)
		{
			int count = 0;
			for (int j = 0; j < size; j++) {
				for (int k = 0; k < size; k++) {
					if (ids[j] == ptIds[k]) {
						count++;
						break;
					}
				}
			}
			if (count == size) return volumeIds[i];
		}
	}
	return (-1);
}

void VolumeMesh::RequestEditStatus()
{
	if (InEditStatus())
	{
		return;
	}
	RequestPointStatus();
	RequestFaceStatus();
	RequestVolumeStatus();
	MakeEditStatusOn();
}

void VolumeMesh::GarbageCollection()
{
	IGsize i, mappedPtId = 0, mappedEdgeId = 0, mappedFaceId = 0;
	igIndex ptIds[32]{}, edgeIds[32]{}, faceIds[32]{}, e[2]{};
	CellArray::Pointer newEdges = CellArray::New();
	CellArray::Pointer newFaces = CellArray::New();
	CellArray::Pointer newFaceEdges = CellArray::New();
	CellArray::Pointer newVolumes = CellArray::New();
	CellArray::Pointer newVolumeEdges = CellArray::New();
	CellArray::Pointer newVolumeFaces = CellArray::New();

	IGsize npts = GetNumberOfPoints();
	IGsize nedges = GetNumberOfEdges();
	IGsize nfaces = GetNumberOfFaces();
	IGsize nvolumes = GetNumberOfVolumes();

	std::vector<igIndex> ptMap(npts);
	std::vector<igIndex> edgeMap(nedges);
	std::vector<igIndex> faceMap(nfaces);

	for (i = 0; i < npts; i++) {
		if (IsPointDeleted(i)) continue;
		if (i != mappedPtId) {
			m_Points->SetPoint(mappedPtId, m_Points->GetPoint(i));
		}
		ptMap[i] = mappedPtId;
		mappedPtId++;
	}
	m_Points->Resize(mappedPtId);

	for (i = 0; i < nedges; i++) {
		if (IsEdgeDeleted(i)) continue;
		m_Edges->GetCellIds(i, e);
		for (int j = 0; j < 2; j++) {
			e[j] = ptMap[e[j]];
		}
		newEdges->AddCellIds(e, 2);
		edgeMap[i] = mappedEdgeId;
		mappedEdgeId++;
	}

	for (i = 0; i < nfaces; i++) {
		if (IsFaceDeleted(i)) continue;
		m_FaceEdges->GetCellIds(i, edgeIds);
		int size = m_Faces->GetCellIds(i, ptIds);
		for (int j = 0; j < size; j++) {
			ptIds[j] = ptMap[ptIds[j]];
			edgeIds[j] = edgeMap[edgeIds[j]];
		}
		newFaces->AddCellIds(ptIds, size);
		newFaceEdges->AddCellIds(edgeIds, size);
		faceMap[i] = mappedFaceId;
		mappedFaceId++;
	}

	for (i = 0; i < nvolumes; i++) {
		if (IsVolumeDeleted(i)) continue;
		int pt_size = m_Volumes->GetCellIds(i, ptIds);
		int edge_size = m_VolumeEdges->GetCellIds(i, edgeIds);
		int face_size = m_VolumeFaces->GetCellIds(i, faceIds);
		for (int j = 0; j < pt_size; j++) {
			ptIds[j] = ptMap[ptIds[j]];
		}
		for (int j = 0; j < edge_size; j++) {
			edgeIds[j] = edgeMap[edgeIds[j]];
		}
		for (int j = 0; j < face_size; j++) {
			faceIds[j] = faceMap[faceIds[j]];
		}
		newVolumes->AddCellIds(ptIds, pt_size);
		newVolumeEdges->AddCellIds(edgeIds, edge_size);
		newVolumeFaces->AddCellIds(faceIds, face_size);
	}

	m_Edges = newEdges;
	m_Faces = newFaces;
	m_FaceEdges = newFaceEdges;
	m_Volumes = newVolumes;
	m_VolumeEdges = newVolumeEdges;
	m_VolumeFaces = newVolumeFaces;

	m_EdgeLinks = nullptr;
	m_FaceLinks = nullptr;
	m_FaceEdgeLinks = nullptr;
	m_VolumeLinks = nullptr;
	m_VolumeEdgeLinks = nullptr;
	m_VolumeFaceLinks = nullptr;

	m_PointDeleteMarker = nullptr;
	m_EdgeDeleteMarker = nullptr;
	m_FaceDeleteMarker = nullptr;
	m_VolumeDeleteMarker = nullptr;
	Modified();
	MakeEditStatusOff();
}

bool VolumeMesh::IsVolumeDeleted(const IGsize volumeId)
{
	return m_VolumeDeleteMarker->IsDeleted(volumeId);
}

IGsize VolumeMesh::AddPoint(const Point& p)
{
	if (!InEditStatus())
	{
		RequestEditStatus();
	}
	IGsize ptId = m_Points->AddPoint(p);

	m_EdgeLinks->AddLink();
	m_FaceLinks->AddLink();
	m_VolumeLinks->AddLink();

	return ptId;
}
IGsize VolumeMesh::AddEdge(const IGsize ptId1, const IGsize ptId2)
{
	igIndex edgeId = GetEdgeIdFormPointIds(ptId1, ptId2);
	if (edgeId == -1) {
		edgeId = GetNumberOfEdges();
		m_Edges->AddCellId2(ptId1, ptId2);
		m_EdgeLinks->AddReference(ptId1, edgeId);
		m_EdgeLinks->AddReference(ptId2, edgeId);
		m_FaceEdgeLinks->AddLink();
		m_VolumeEdgeLinks->AddLink();
		m_EdgeDeleteMarker->AddTag();
	}
	return edgeId;
}
IGsize VolumeMesh::AddFace(igIndex* ptIds, int size)
{
	igIndex edgeIds[64]{};
	for (int i = 0; i < size; i++) {
		edgeIds[i] = AddEdge(ptIds[i], ptIds[(i + 1) % size]);
	}
	igIndex faceId = GetFaceIdFormPointIds(ptIds, size);
	if (faceId == -1) {
		faceId = GetNumberOfFaces();
		m_Faces->AddCellIds(ptIds, size);
		m_FaceEdges->AddCellIds(edgeIds, size);
		for (int i = 0; i < size; i++) {
			m_FaceLinks->AddReference(ptIds[i], faceId);
			m_FaceEdgeLinks->AddReference(edgeIds[i], faceId);
		}
		m_VolumeFaceLinks->AddLink();
		m_FaceDeleteMarker->AddTag();
	}
	return faceId;
}

void VolumeMesh::DeletePoint(const IGsize ptId) {
	if (!InEditStatus())
	{
		RequestEditStatus();
	}
	if (IsPointDeleted(ptId))
	{
		return;
	}
	igIndex edgeIds[64]{};
	int size = GetPointToNeighborEdges(ptId, edgeIds);
	for (int i = 0; i < size; i++) {
		DeleteEdge(edgeIds[i]);
	}
	m_EdgeLinks->DeleteLink(ptId);
	m_FaceLinks->DeleteLink(ptId);
	m_VolumeLinks->DeleteLink(ptId);
	m_PointDeleteMarker->MarkDeleted(ptId);
}

void VolumeMesh::DeleteEdge(const IGsize edgeId)
{
	if (!InEditStatus())
	{
		RequestEditStatus();
	}
	if (IsEdgeDeleted(edgeId))
	{
		return;
	}
	igIndex faceIds[64]{}, e[2]{};
	GetEdgePointIds(edgeId, e);
	for (int i = 0; i < 2; i++) {
		m_EdgeLinks->RemoveReference(e[i], edgeId);
	}

	int size = GetEdgeToNeighborFaces(edgeId, faceIds);
	for (int i = 0; i < size; i++) {
		DeleteFace(faceIds[i]);
	}
	m_FaceEdgeLinks->DeleteLink(edgeId);
	m_VolumeEdgeLinks->DeleteLink(edgeId);
	m_EdgeDeleteMarker->MarkDeleted(edgeId);
}
void VolumeMesh::DeleteFace(const IGsize faceId) {
	if (!InEditStatus())
	{
		RequestEditStatus();
	}
	if (IsFaceDeleted(faceId))
	{
		return;
	}
	igIndex ptIds[32]{}, edgeIds[32]{}, volumeIds[128];
	int size = GetFacePointIds(faceId, ptIds);
	GetFaceEdgeIds(faceId, edgeIds);
	for (int i = 0; i < size; i++) {
		m_FaceLinks->RemoveReference(ptIds[i], faceId);
		m_FaceEdgeLinks->RemoveReference(edgeIds[i], faceId);
	}

	size = GetFaceToNeighborVolumes(faceId, volumeIds);
	for (int i = 0; i < size; i++) {
		DeleteVolume(volumeIds[i]);
	}
	m_FaceDeleteMarker->MarkDeleted(faceId);
}

void VolumeMesh::DeleteVolume(const IGsize volumeId) {
	if (!InEditStatus())
	{
		RequestEditStatus();
	}
	if (IsVolumeDeleted(volumeId))
	{
		return;
	}
	igIndex ptIds[32]{}, edgeIds[32]{}, faceIds[32]{}, volumeIds[128];
	int pt_size = GetVolumePointIds(volumeId, ptIds);
	int edge_size = GetVolumeEdgeIds(volumeId, edgeIds);
	int face_size = GetVolumeFaceIds(volumeId, faceIds);
	for (int i = 0; i < pt_size; i++) {
		m_VolumeLinks->RemoveReference(ptIds[i], volumeId);
	}
	for (int i = 0; i < edge_size; i++) {
		m_VolumeEdgeLinks->RemoveReference(ptIds[i], volumeId);
	}
	for (int i = 0; i < face_size; i++) {
		m_VolumeFaceLinks->RemoveReference(ptIds[i], volumeId);
	}
	m_VolumeDeleteMarker->MarkDeleted(volumeId);
}

VolumeMesh::VolumeMesh()
{
	m_ViewStyle = IG_SURFACE;
};

void VolumeMesh::RequestFaceStatus()
{
	if (m_Faces == nullptr ||
		(m_Faces->GetMTime() < m_Volumes->GetMTime()))
	{
		BuildFaceAndEdges();
	}

	if (m_FaceEdgeLinks == nullptr ||
		(m_FaceEdgeLinks->GetMTime() < m_FaceEdges->GetMTime()))
	{
		BuildFaceEdgeLinks();
	}

	if (m_FaceLinks == nullptr ||
		(m_FaceLinks->GetMTime() < m_Faces->GetMTime()))
	{
		BuildFaceLinks();
	}

	if (m_EdgeLinks == nullptr ||
		(m_EdgeLinks->GetMTime() < m_Edges->GetMTime()))
	{
		BuildEdgeLinks();
	}

	if (m_EdgeDeleteMarker == nullptr)
	{
		m_EdgeDeleteMarker = DeleteMarker::New();
	}
	m_EdgeDeleteMarker->Initialize(GetNumberOfEdges());

	if (m_FaceDeleteMarker == nullptr)
	{
		m_FaceDeleteMarker = DeleteMarker::New();
	}
	m_FaceDeleteMarker->Initialize(GetNumberOfFaces());
}

void VolumeMesh::RequestVolumeStatus()
{
	if (m_VolumeLinks == nullptr ||
		(m_VolumeLinks->GetMTime() < m_Volumes->GetMTime()))
	{
		BuildVolumeLinks();
	}

	if (m_VolumeEdgeLinks == nullptr ||
		(m_VolumeEdgeLinks->GetMTime() < m_VolumeEdges->GetMTime()))
	{
		BuildVolumeEdgeLinks();
	}

	if (m_VolumeFaceLinks == nullptr ||
		(m_VolumeFaceLinks->GetMTime() < m_VolumeFaces->GetMTime()))
	{
		BuildVolumeFaceLinks();
	}

	if (m_VolumeDeleteMarker == nullptr)
	{
		m_VolumeDeleteMarker = DeleteMarker::New();
	}
	m_VolumeDeleteMarker->Initialize(GetNumberOfEdges());
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

	m_Positions = m_Points->ConvertToArray();
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

	if (this->GetFaces() == nullptr)
	{
		this->BuildFaceAndEdges();
	}

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