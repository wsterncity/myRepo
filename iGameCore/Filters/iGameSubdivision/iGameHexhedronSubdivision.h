#ifndef HexhedronSubdivision_h
#define HexhedronSubdivision_h

#include "iGameFilter.h"
#include "iGameVolumeMesh.h"

IGAME_NAMESPACE_BEGIN
class HexhedronSubdivision : public Filter {
public:
	I_OBJECT(HexhedronSubdivision);
	static Pointer New() { return new HexhedronSubdivision; }

	bool Execute()override;

	void SetMesh(VolumeMesh::Pointer mesh) {
		this->mesh = mesh;
	}
protected:
	VolumeMesh::Pointer mesh{};
	HexhedronSubdivision() {
		mesh = nullptr;
	};
	HexhedronSubdivision(VolumeMesh::Pointer _mesh) {
		this->mesh = _mesh;
	}
};
IGAME_NAMESPACE_END
#endif