#ifndef MVCMapping_h
#define MVCMapping_h
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include <Eigen/dense>
#include <Eigen/sparse>

typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> T;

IGAME_NAMESPACE_BEGIN
class MVCMapping:public Filter
{
public:
	I_OBJECT(MVCMapping);
	static MVCMapping::Pointer New(){return new MVCMapping;}
	bool Execute() override
	{
		mesh = DynamicCast<SurfaceMesh>(GetInput(0));
		mesh->RequestEditStatus();
		IGsize vnum = mesh->GetNumberOfPoints();
		IGsize fnum = mesh->GetNumberOfFaces();
		std::vector<T> coefficients;
		SpMat mat(vnum, vnum);
		Eigen::VectorXd bx(vnum);
		Eigen::VectorXd by(vnum);
		Eigen::VectorXd bz(vnum);
		std::vector<igIndex> boundary_vid;

		CalcEdgeNorm();

		//coeff matrix
		for(igIndex fid=0; fid<fnum; fid++)
		{
			igIndex pids[3];
			mesh->GetFacePointIds(fid, pids);
			for(int i = 0; i < 3; i++)
			{
				if(mesh->IsBoundaryPoint(pids[i]))
					continue;
				igIndex eid0_1 = mesh->GetEdgeIdFormPointIds(pids[i], pids[(i+1)%3]);
				igIndex eid0_2 = mesh->GetEdgeIdFormPointIds(pids[i], pids[(i+2)%3]);
				assert((eid0_1 != -1) && (eid0_2 != -1));
				Vector3f vec0_1 = mesh->GetPoint(pids[(i+1)%3]) - mesh->GetPoint(pids[i]);
				Vector3f vec0_2 = mesh->GetPoint(pids[(i+2)%3]) - mesh->GetPoint(pids[i]);
				double angle = std::acos(vec0_1.dot(vec0_2) / (EdgeNorm[eid0_1] * EdgeNorm[eid0_2]));
				double w0_1 = std::tan(0.5 * angle) / EdgeNorm[eid0_1];
				double w0_2 = std::tan(0.5 * angle) / EdgeNorm[eid0_2];

				coefficients.emplace_back(pids[i], pids[i], w0_1);
				coefficients.emplace_back(pids[i], pids[i], w0_2);
				
				coefficients.emplace_back(pids[i], pids[(i+1)%3], -w0_1);
				coefficients.emplace_back(pids[i], pids[(i+2)%3], -w0_2);
			}
		}

		for(igIndex vid = 0; vid < vnum; vid++)
			if(mesh->IsBoundaryPoint(vid))
				coefficients.emplace_back(vid, vid, 1.0);

		mat.setFromTriplets(coefficients.begin(), coefficients.end());
		Eigen::SparseLU<SpMat, Eigen::COLAMDOrdering<int>> solver;
		solver.compute(mat);
		assert(solver.info() == Eigen::Success);


		//constant term
		for(igIndex vid = 0; vid < vnum; vid++)
		{
			if(mesh->IsBoundaryPoint(vid))
				boundary_vid.push_back(vid);
			else
			{
				bx(vid) = 0.0;
				by(vid) = 0.0;
				bz(vid) = 0.0;
			}
		}

		double area_sum = 0;
		for(igIndex fid=0; fid<fnum; fid++)
		{
			igIndex pids[3];
			mesh->GetFacePointIds(fid, pids);
			Point p[3];
			for(int i=0;i<3;i++) p[i] = mesh->GetPoint(pids[i]);
			Vector2d vec0_1 = p[1] - p[0];
			Vector2d vec0_2 = p[2] - p[0];
			area_sum += vec0_1.cross(vec0_2).norm() / 2;
		}

		double circle_radian_speed = (2 * M_PI) / boundary_vid.size();
		double area_1_factor = sqrt(area_sum / M_PI);
		std::vector<bool> isIncluded;
		isIncluded.resize(boundary_vid.size(), false);

		int cur = 0;
		for(int step = 0;step < boundary_vid.size();step ++)
		{
			isIncluded[cur] = true;
			bx(boundary_vid[cur]) = area_1_factor * std::cos(step * circle_radian_speed);
			by(boundary_vid[cur]) = area_1_factor * std::sin(-step * circle_radian_speed);
			bz(boundary_vid[cur]) = 0.0;
			for(int i = 0; i < boundary_vid.size(); i++)
			{
				if(!isIncluded[i] && mesh->GetEdgeIdFormPointIds(boundary_vid[cur], boundary_vid[i]) != -1)
				{
					cur = i;
					break;
				}
			}
		}


		Eigen::VectorXd x = solver.solve(bx);
		Eigen::VectorXd y = solver.solve(by);
		Eigen::VectorXd z = solver.solve(bz);
		assert(solver.info() == Eigen::Success);

#pragma omp parallel for
		for(igIndex vid = 0; vid < vnum; vid++)
			mesh->SetPoint(vid, Point(x(vid), y(vid), z(vid) ) );
		
		mesh->Modified();
		return true;
	
	}

protected:
	MVCMapping()
	{
		SetNumberOfInputs(1);
		SetNumberOfOutputs(0);
	}

	~MVCMapping() override = default;

	SurfaceMesh::Pointer mesh;
	std::vector<double> EdgeNorm;
	
	
	void CalcEdgeNorm()
	{
		IGsize e_num = mesh->GetNumberOfEdges();
		EdgeNorm.resize(e_num, 0);
		for(igIndex ei = 0; ei < e_num; ei ++)
		{
			igIndex pids[2];
			mesh->GetEdgePointIds(ei, pids);
			EdgeNorm[ei] = (mesh->GetPoint(pids[0]) - mesh->GetPoint(pids[1])).norm();
		}
	}


};



IGAME_NAMESPACE_END
#endif // ! MVCMapping_h
