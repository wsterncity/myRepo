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
		Eigen::VectorXd bx(vnum);
		Eigen::VectorXd by(vnum);
		Eigen::VectorXd bz(vnum);
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
				bx(vid) = 0.0;
				by(vid) = 0.0;
				bz(vid) = 0.0;
				igIndex adj_vid[64];
				IGsize adj_vnum = mesh->GetPointToOneRingPoints(vid, adj_vid);
				coefficients.push_back(T(vid, vid, adj_vnum));
				for(int i=0;i<adj_vnum;i++)
					coefficients.push_back(T(vid, adj_vid[i], -1));

			}
		
		}

		float circle_radian_speed = (2 * PI) / boundary_vid.size();
		std::vector<bool> isIncluded;
		isIncluded.resize(boundary_vid.size(), false);
		int step = 0;
		int cur = 0;
		while(step < boundary_vid.size())
		{
			isIncluded[cur] = true;
			bx(boundary_vid[cur]) = std::cos(step * circle_radian_speed);
			by(boundary_vid[cur]) = std::sin(step * circle_radian_speed);
			bz(boundary_vid[cur]) = 0.0;
			step ++;
			for(int i = 0; i < boundary_vid.size(); i++)
			{
				if(!isIncluded[i] && mesh->GetEdgeIdFormPointIds(boundary_vid[cur], boundary_vid[i]) != -1)
				{
					cur = i;
					break;
				}
			}
		}

		mat.setFromTriplets(coefficients.begin(), coefficients.end());
		Eigen::SparseLU<SpMat, Eigen::COLAMDOrdering<int>> solver;
		solver.compute(mat);
		assert(solver.info() == Eigen::Success);
		Eigen::VectorXd x = solver.solve(bx);
		Eigen::VectorXd y = solver.solve(by);
		Eigen::VectorXd z = solver.solve(bz);
		assert(solver.info() == Eigen::Success);
		
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