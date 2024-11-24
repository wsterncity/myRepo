#ifndef Morphing_h
#define Morphing_h
#include <Eigen/sparse>
#include <Eigen/SVD>
#include "iGameFilter.h"
#include "iGamePointSet.h"
#include "iGameSurfaceMesh.h"


IGAME_NAMESPACE_BEGIN
class Morphing : public Filter
{
public:
	I_OBJECT(Morphing);
	static Pointer New() { return new Morphing; }

	void SetInterpolation(double d)
	{
		assert(d >=0 && d <= 1.0);
		interpolation = d;
	}

	void SetPainter(Painter::Pointer p)
	{
		painter = p;
		painter->SetPen(10);
		painter->SetPen(Color::Red);
	}

	void Init()
	{
		sourceMesh = DynamicCast<SurfaceMesh>(GetInput(0));
		targetMesh = DynamicCast<SurfaceMesh>(GetInput(1));
		sourceMesh->RequestEditStatus();
		targetMesh->RequestEditStatus();
		assert(sourceMesh->GetNumberOfPoints() == targetMesh->GetNumberOfPoints() && 
			sourceMesh->GetNumberOfEdges() == targetMesh->GetNumberOfEdges() &&
			sourceMesh->GetNumberOfFaces() == targetMesh->GetNumberOfFaces() && "Input error.");
		int fnum = sourceMesh->GetNumberOfFaces();
		int vnum = sourceMesh->GetNumberOfPoints();

		fixIdx = 0;
		Point p = targetMesh->GetPoint(fixIdx);
		painter->Show(painter->DrawPoint(p));

		S.resize(fnum);
		angle.resize(fnum);
		std::vector<Eigen::Triplet<double>> global_coeff;

		for(igIndex fi=0;fi<fnum;fi++)
		{
			igIndex sourceTriVertIds[3], targetTriVertIds[3];
			Point sourceTriVert[3], targetTriVert[3];
			Eigen::Matrix<double, 3, 2> _sourceTriVert, _targetTriVert;

			sourceMesh->GetFacePointIds(fi, sourceTriVertIds);
			targetMesh->GetFacePointIds(fi, targetTriVertIds);

			for(int i=0;i<3;i++)
			{
				sourceTriVert[i] = sourceMesh->GetPoint(sourceTriVertIds[i]);
				targetTriVert[i] = targetMesh->GetPoint(targetTriVertIds[i]);
			}

			for(int i=0;i<3;i++)
			{
				_sourceTriVert.row(i) << sourceTriVert[i][0], sourceTriVert[i][1];
				_targetTriVert.row(i) << targetTriVert[i][0], targetTriVert[i][1];
			}

			PolarDecomposition(_sourceTriVert, _targetTriVert, fi);

			for (int i = 0; i < 3; i++)
			{
				auto eid = sourceMesh->GetEdgeIdFormPointIds(sourceTriVertIds[i], sourceTriVertIds[(i + 1) % 3]);
				auto vec1 = sourceTriVert[(i + 1) % 3] - sourceTriVert[(i + 2) % 3];
				auto vec2 = sourceTriVert[i] - sourceTriVert[(i + 2) % 3];
				double cot = vec1.dot(vec2) / vec1.cross(vec2).norm();

				if(sourceTriVertIds[i] != fixIdx)
				{
					global_coeff.emplace_back(sourceTriVertIds[i], sourceTriVertIds[i], cot);
					global_coeff.emplace_back(sourceTriVertIds[i], sourceTriVertIds[(i + 1) % 3], -cot);
				}

				if(sourceTriVertIds[(i + 1) % 3] != fixIdx)
				{
					global_coeff.emplace_back(sourceTriVertIds[(i + 1) % 3], sourceTriVertIds[(i + 1) % 3], cot);
					global_coeff.emplace_back(sourceTriVertIds[(i + 1) % 3], sourceTriVertIds[i], -cot);
				}
			}
		}

		global_coeff.emplace_back( fixIdx, fixIdx, 1.0);
		Eigen::SparseMatrix<double> global_coeff_mat;
		global_coeff_mat.resize(vnum, vnum);
		global_coeff_mat.setFromTriplets(global_coeff.begin(), global_coeff.end());
		
		global_solver.compute(global_coeff_mat);
		assert(global_solver.info() == Eigen::Success);
	}

	bool Execute() override 
	{
		int vnum = sourceMesh->GetNumberOfPoints();
		int fnum = sourceMesh->GetNumberOfFaces();
		//Construct constraints
		Eigen::MatrixX2d UV;
		Eigen::VectorXd bu, bv;
		UV.resize(vnum, 2);
		bu.setZero(vnum);
		bv.setZero(vnum);
		for(int fid = 0; fid < fnum; fid++)
		{
			Eigen::Matrix2d R_t, S_t, I;
			I.setIdentity();
			double angle_t = interpolation * angle[fid];
			R_t << cos(angle_t), -sin(angle_t), sin(angle_t), cos(angle_t);
			S_t = (1 - interpolation) * I + interpolation * S[fid];
			Eigen::Matrix2d At = R_t * S_t;
			igIndex pids[3];
			Point p[3];
			sourceMesh->GetFacePointIds(fid, pids);
			for(int i=0;i<3;i++) p[i] = sourceMesh->GetPoint(pids[i]);
			std::vector<Eigen::Vector2d> e;
			e.resize(3);

			e[0] << p[1][0] - p[0][0], p[1][1] - p[0][1];
			e[1] << p[2][0] - p[1][0], p[2][1] - p[1][1];
			e[2] << p[0][0] - p[2][0], p[0][1] - p[2][1];
			for(int i=0;i<3;i++)
			{
				auto eid = sourceMesh->GetEdgeIdFormPointIds(pids[i], pids[(i+1)%3]);
				auto vec1 = p[(i+1)%3] - p[(i+2)%3];
				auto vec2 = p[i] - p[(i+2)%3];
				double cot = vec1.dot(vec2) / vec1.cross(vec2).norm();
				Eigen::Vector2d uv = cot * At * e[i];
				if(pids[i] != fixIdx)
				{
					bu[pids[i]] -= uv[0];
					bv[pids[i]] -= uv[1];
				}

				if(pids[(i+1)%3] != fixIdx)
				{
					bu[pids[(i + 1) % 3]] += uv[0];
					bv[pids[(i + 1) % 3]] += uv[1];
				}
			}

		}

		auto fixPoint = targetMesh->GetPoint(fixIdx);
		bu[0] = fixPoint[0];
		bv[0] = fixPoint[1];
		UV.col(0) = global_solver.solve(bu);
		UV.col(1) = global_solver.solve(bv);

#pragma omp parallel for
		for(igIndex pid = 0; pid < vnum; pid ++)
			sourceMesh->SetPoint(pid, Point(UV(pid, 0), UV(pid, 1), 0) );

		sourceMesh->Modified();
		return true;
	}


protected:
	igIndex fixIdx = 0;
	SurfaceMesh::Pointer sourceMesh, targetMesh;
	Eigen::SparseLU<Eigen::SparseMatrix<double>> global_solver;
	std::vector<Eigen::Matrix2d> S;
	std::vector<double> angle;
	double interpolation = 1.0;
	Painter::Pointer painter;

	Morphing() {
		SetNumberOfInputs(2);
		SetNumberOfOutputs(0);
	}

	~Morphing() noexcept override = default;

	void PolarDecomposition(Eigen::Matrix<double, 3, 2> source, Eigen::Matrix<double, 3, 2> target, igIndex fi)
	{
		Eigen::Matrix<double, 2, 2> mSource, mTarget;
		mTarget << target(1, 0) - target(0, 0), target(2, 0) - target(0, 0),
				target(1, 1) - target(0, 1), target(2, 1) - target(0, 1);
		mSource << source(1, 0) - source(0, 0), source(2, 0) - source(0, 0),
			source(1, 1) - source(0, 1), source(2, 1) - source(0, 1);
		Eigen::Matrix<double, 2, 2> jacobiMatrix =  mTarget * mSource.inverse();
		Eigen::JacobiSVD<Eigen::Matrix2d> svd(jacobiMatrix, Eigen::ComputeFullU | Eigen::ComputeFullV);
		Eigen::Matrix2d U = svd.matrixU();
		Eigen::Matrix2d V = svd.matrixV();
		Eigen::Matrix2d D = svd.singularValues().asDiagonal();
		Eigen::Matrix2d R = U * V.transpose();
		S[fi] = V * D * V.transpose();
		angle[fi] = atan2(R(1,0),R(1,1));
	}

};


IGAME_NAMESPACE_END



#endif