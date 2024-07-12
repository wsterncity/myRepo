#include "iGameVolumeMesh.h"

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
	igIndex i, ncell;
	igIndex nfaces = this->GetNumberOfFaces();

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
IGAME_NAMESPACE_END