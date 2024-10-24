#ifndef TutteMapping_h
#define TutteMapping_h
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include<Eigen/Sparse>

#define PI 3.14159265359
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> T;

IGAME_NAMESPACE_BEGIN
class TutteMapping : public Filter{
public:
	I_OBJECT(TutteMapping);
	static Pointer New(){return new TutteMapping;}
	bool Execute() override
	{
		SurfaceMesh::Pointer mesh = DynamicCast<SurfaceMesh>(GetInput(0));
		IGsize vnum = mesh->GetNumberOfPoints();
		mesh->RequestEditStatus();
		
		std::vector<T> coefficients;
		SpMat mat(vnum, vnum);
		Eigen::VectorXf bx(vnum);
		Eigen::VectorXf by(vnum);
		Eigen::VectorXf bz(vnum);
		std::vector<igIndex> boundary_vid;

		//construct linear system
		for(igIndex vid = 0; vid < vnum; vid++)
		{
			if(mesh->IsBoundaryPoint(vid))
			{
				coefficients.push_back(T(vid, vid, 1.0));
				boundary_vid.push_back(vid);
			}
			else
			{
				bx(vid) = 0;
				by(vid) = 0;
				bz(vid) = 0;
				igIndex adj_vid[64];
				IGsize adj_vnum = mesh->GetPointToOneRingPoints(vid, adj_vid);
				coefficients.push_back(T(vid, vid, adj_vnum));
				for(int i=0;i<adj_vnum;i++)
					coefficients.push_back(T(vid, adj_vid[i], -1));

			}
		
		}

		float circle_radian_speed = (2 * PI) / boundary_vid.size();
		for(float step = 0; step < boundary_vid.size(); step ++)
		{
			bx(boundary_vid[step]) = std::cos(step * circle_radian_speed);
			by(boundary_vid[step]) = std::sin(step * circle_radian_speed);
			bz(boundary_vid[step]) = 0;
		}

		mat.setFromTriplets(coefficients.begin(), coefficients.end());
		Eigen::SparseLU<SpMat, Eigen::COLAMDOrdering<int>> solver;
		solver.analyzePattern(mat);
		solver.factorize(mat);
		assert(solver.info() == Eigen::Success);
		Eigen::VectorXf x = solver.solve(bx);
		Eigen::VectorXf y = solver.solve(by);
		Eigen::VectorXf z = solver.solve(bz);
		assert(x.size() == vnum);
		
		for(igIndex vid = 0; vid < vnum; vid++)
			mesh->SetPoint(vid, Point(x(vid), y(vid), z(vid) ) );
		mesh->Modified();
		return true;
	}

protected:
	TutteMapping()
	{
		SetNumberOfInputs(1);
		SetNumberOfOutputs(0);
	}
	~TutteMapping() noexcept override = default;
};

IGAME_NAMESPACE_END
#endif