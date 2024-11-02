#include "iGameARAPMapping.h"

IGAME_NAMESPACE_BEGIN

bool ARAPMapping::Execute()
{
	mesh = DynamicCast<SurfaceMesh>(GetInput(0));
	mesh->RequestEditStatus();
	IGsize fnum = mesh->GetNumberOfFaces();
	IGsize vnum = mesh->GetNumberOfPoints();

	std::vector<Eigen::Matrix2d> Lts;
	
	Eigen::MatrixXd local_coord;
	//Construct the coeffitients matix of global step and the local coordinates of every 3d triangle. 

	Lts.resize(fnum);
	local_coord.resize(fnum, 6);
	std::vector<Eigen::Triplet<double>> global_coeff;
	

#pragma omp parallel for
	for(igIndex fid = 0; fid < fnum; fid ++)
	{
		igIndex pids[3];
		Point p[3];
		mesh->GetFacePointIds(fid, pids);
		for(int i=0;i<3;i++)
			p[i] = mesh->GetPoint(pids[i]);


		Vector3f v0_v1 = p[1] - p[0];
		Vector3f v0_v2 = p[2] - p[0];
		Vector3f normal = mesh->GetFace(fid)->GetNormal();
		Vector3f v0_v1_norm = v0_v1.normalized();
		Vector3f identity_h = normal.cross(v0_v1_norm);
		local_coord.row(fid) << 0, 0, v0_v1.norm(), 0, v0_v2.dot(v0_v1_norm), v0_v2.dot(identity_h);
	}

	for(igIndex fid = 0; fid < fnum; fid ++)
	{
		igIndex pids[3];
		Point p[3];
		mesh->GetFacePointIds(fid, pids);
		for(int i=0;i<3;i++) p[i] = mesh->GetPoint(pids[i]);
		for (int i = 0; i < 3; i++)
		{
			auto eid = mesh->GetEdgeIdFormPointIds(pids[i], pids[(i + 1) % 3]);
			auto vec1 = p[(i + 1) % 3] - p[(i + 2) % 3];
			auto vec2 = p[i] - p[(i + 2) % 3];
			double cot = vec1.dot(vec2) / vec1.cross(vec2).norm();
			global_coeff.emplace_back(pids[i], pids[i], cot);
			global_coeff.emplace_back(pids[(i + 1) % 3], pids[(i + 1) % 3], cot);
			global_coeff.emplace_back(pids[i], pids[(i + 1) % 3], -cot);
			global_coeff.emplace_back(pids[(i + 1) % 3], pids[i], -cot);
		}
	}

	Eigen::SparseMatrix<double> global_coeff_mat;
	global_coeff_mat.resize(vnum, vnum);
	global_coeff_mat.setFromTriplets(global_coeff.begin(), global_coeff.end());

	Eigen::SparseLU<Eigen::SparseMatrix<double>> global_solver;
	global_solver.compute(global_coeff_mat);
	assert(global_solver.info() == Eigen::Success);

	Eigen::MatrixX2d UV;
	UV.resize(vnum,2);
	Tutte(UV);

	int IterNum = 100;
	for(int i=0;i < IterNum;i++)
	{
		UpdateProgress((double)(i + 1) / (double)IterNum);

#pragma omp parallel for
		for(igIndex fid = 0; fid < fnum; fid++)
		{
			igIndex pids[3];
			mesh->GetFacePointIds(fid, pids);

			Eigen::Matrix2d J, P, S;
			P << UV(pids[1], 0) - UV(pids[0], 0), UV(pids[2], 0) - UV(pids[0], 0),
				UV(pids[1], 1) - UV(pids[0], 1), UV(pids[2], 1) - UV(pids[0], 1);
			S << local_coord(fid, 2) - local_coord(fid, 0), local_coord(fid, 4) - local_coord(fid, 0),
				local_coord(fid, 3) - local_coord(fid, 1), local_coord(fid, 5) - local_coord(fid, 1);
			J = P * S.inverse();
			J = P * S.inverse();

			Eigen::JacobiSVD<Eigen::Matrix2d> svd(J, Eigen::ComputeFullU | Eigen::ComputeFullV);

			Eigen::Matrix2d U = svd.matrixU();
			Eigen::Matrix2d V = svd.matrixV();

			Eigen::Matrix2d R = U * V.transpose();

			if (R.determinant() < 0)
			{
				U(0, 1) = -U(0, 1);
				U(1, 1) = -U(1, 1);
				R = U * V.transpose();
			}
			Lts[fid] = R;

		}

		Eigen::VectorXd bu, bv;
		bu.setZero(vnum);
		bv.setZero(vnum);
		for(int fid=0;fid<fnum;fid++)
		{
			igIndex pids[3];
			Point p[3];
			mesh->GetFacePointIds(fid, pids);
			for(int i=0;i<3;i++) p[i] = mesh->GetPoint(pids[i]);
			std::vector<Eigen::Vector2d> e;
			e.resize(3);

			e[0] << local_coord(fid, 2), local_coord(fid, 3);
			e[1] << local_coord(fid, 4) - local_coord(fid, 2),  local_coord(fid, 5) - local_coord(fid, 3);
			e[2] << -local_coord(fid, 4), -local_coord(fid, 5);
			for(int i=0;i<3;i++)
			{
				auto eid = mesh->GetEdgeIdFormPointIds(pids[i], pids[(i+1)%3]);
				auto vec1 = p[(i+1)%3] - p[(i+2)%3];
				auto vec2 = p[i] - p[(i+2)%3];
				double cot = vec1.dot(vec2) / vec1.cross(vec2).norm();
				Eigen::Vector2d uv = cot * Lts[fid] * e[i];
				bu[pids[i]] -= uv[0];
				bv[pids[i]] -= uv[1];
				bu[pids[(i+1)%3]] += uv[0];
				bv[pids[(i+1)%3]] += uv[1];
			}

		}
		UV.col(0) = global_solver.solve(bu);
		UV.col(1) = global_solver.solve(bv);
	}

	for(igIndex pid=0; pid < mesh->GetNumberOfPoints(); pid ++)
		mesh->SetPoint(pid, Point(UV(pid, 0),UV(pid, 1), 0));
	mesh->Modified();

	return true;
}


void ARAPMapping::Tutte(Eigen::MatrixX2d& UV)
{
	// Tutte paramization to get (u,v)
	IGsize fnum = mesh->GetNumberOfFaces();
	IGsize vnum = mesh->GetNumberOfPoints();
	std::vector<Eigen::Triplet<double>> coefficients;
	Eigen::SparseMatrix<double> mat(vnum, vnum);
	Eigen::VectorXd bx(vnum);
	Eigen::VectorXd by(vnum);
	std::vector<igIndex> boundary_vid;
	
	double area_sum = 0;
	for(igIndex fid=0;fid<fnum;fid++)
	{
		igIndex pids[3];
		mesh->GetFacePointIds(fid, pids);
		Point p[3];
		for(int i=0;i<3;i++) p[i] = mesh->GetPoint(pids[i]);
		Vector2d vec0_1 = p[1] - p[0];
		Vector2d vec0_2 = p[2] - p[0];
		area_sum += vec0_1.cross(vec0_2).norm() / 2;
	}
	
	//construct the linear system
	for(igIndex vid = 0; vid < vnum; vid++)
	{
		if(mesh->IsBoundaryPoint(vid))
		{
			coefficients.push_back(Eigen::Triplet<double>(vid, vid, 1.0));
			boundary_vid.push_back(vid);
		}
		else
		{
			bx(vid) = 0.0;
			by(vid) = 0.0;
			igIndex adj_vid[64];
			IGsize adj_vnum = mesh->GetPointToOneRingPoints(vid, adj_vid);
			coefficients.push_back(Eigen::Triplet<double>(vid, vid, adj_vnum));
			for(int i=0;i<adj_vnum;i++)
				coefficients.push_back(Eigen::Triplet<double>(vid, adj_vid[i], -1));

		}

	}

	float circle_radian_speed = (2 * M_PI) / boundary_vid.size();
	std::vector<bool> isIncluded;
	isIncluded.resize(boundary_vid.size(), false);
	double area_1_factor = sqrt(area_sum / M_PI);
	int cur = 0;
	for(int step = 0; step < boundary_vid.size();step ++)
	{
		isIncluded[cur] = true;
		bx(boundary_vid[cur]) = area_1_factor * std::cos(step * circle_radian_speed);
		by(boundary_vid[cur]) = area_1_factor * std::sin(-step * circle_radian_speed);
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
	Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
	solver.compute(mat);
	assert(solver.info() == Eigen::Success);
	UV.col(0) = solver.solve(bx);
	UV.col(1) = solver.solve(by);
	assert(solver.info() == Eigen::Success);
}

IGAME_NAMESPACE_END