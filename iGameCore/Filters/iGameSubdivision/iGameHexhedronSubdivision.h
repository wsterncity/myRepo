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
	Point B_spline(Point p[4], float t);
	Point BSplineSurface(Point ControlPts[4][4][4], double u, double v, double w);
	void SetMesh(VolumeMesh::Pointer mesh) {
		this->mesh = mesh;
	}
protected:
	VolumeMesh::Pointer mesh{};
	HexhedronSubdivision() {
		mesh = nullptr;
		this->SetNumberOfOutputs(1);	
		this->SetNumberOfInputs(1);
	};
	HexhedronSubdivision(VolumeMesh::Pointer _mesh) {
		this->mesh = _mesh;
	}
};
IGAME_NAMESPACE_END
#endif