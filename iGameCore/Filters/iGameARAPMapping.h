#ifndef ARAP_h
#define APAP_h
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include <Eigen/dense>
#include <Eigen/sparse>
#include <Eigen/SVD>

IGAME_NAMESPACE_BEGIN
class ARAPMapping : public Filter
{
public:
	I_OBJECT(ARAPMapping);
	static ARAPMapping::Pointer New(){return new ARAPMapping;}
	bool Execute() override;

protected:
	ARAPMapping()
	{
		SetNumberOfInputs(1);
		SetNumberOfOutputs(0);
	}

	~ARAPMapping() override = default;

	SurfaceMesh::Pointer mesh;

	void Tutte(Eigen::MatrixX2d& UV);
};


IGAME_NAMESPACE_END

#endif // !ARAP_h
