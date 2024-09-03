#ifndef QuadSubdivision_h
#define QuadSubdivision_h

#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"

IGAME_NAMESPACE_BEGIN
class QuadSubdivision : public Filter {
public:
	I_OBJECT(QuadSubdivision);
	static Pointer New() { return new QuadSubdivision; }

	bool Execute()override;
	void SetMesh(SurfaceMesh::Pointer mesh) {
		this->mesh = mesh;
	}
protected:
	SurfaceMesh::Pointer mesh{};
	QuadSubdivision() {
		mesh = nullptr;
		this->SetNumberOfOutputs(1);
		this->SetNumberOfInputs(1);
	};
	QuadSubdivision(SurfaceMesh::Pointer _mesh) {
		this->mesh = _mesh;
	}
};
IGAME_NAMESPACE_END
#endif