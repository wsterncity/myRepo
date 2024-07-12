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
};
IGAME_NAMESPACE_END
#endif