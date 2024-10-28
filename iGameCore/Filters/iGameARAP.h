#ifndef ARAP_h
#define APAP_h
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include <Eigen/dense>
#include <Eigen/sparse>
#include <Eigen/SVD>

IGAME_NAMESPACE_BEGIN
class ARAP : public Filter
{
public:
	I_OBJECT(ARAP);
	static ARAP::Pointer New(){return new ARAP;}
	bool Execute() override;

protected:
	ARAP()
	{
		SetNumberOfInputs(1);
		SetNumberOfOutputs(0);
	}

	~ARAP() override = default;

	SurfaceMesh::Pointer mesh;
	std::vector<Eigen::Matrix2d> Lts;
	Eigen::MatrixX2d UV;
	Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> global_solver;
	Eigen::MatrixXd local_coord;
	void local();
	void global();
	void Initial();
};


IGAME_NAMESPACE_END

#endif // !ARAP_h
