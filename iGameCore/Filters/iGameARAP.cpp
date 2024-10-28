#include "iGameARAP.h"

IGAME_NAMESPACE_BEGIN

bool ARAP::Execute()
{
	mesh = DynamicCast<SurfaceMesh>(GetInput(0));
	mesh->RequestEditStatus();
	Initial();
	
	for(int i=0;i < 10;i++)
	{
		UpdateProgress((double)(i + 1)/100.0);
		local();
		global();
	}

	for(igIndex pid=0; pid < mesh->GetNumberOfPoints(); pid ++)
		mesh->SetPoint(pid, Point(UV(pid, 0),UV(pid, 1), 0));
	mesh->Modified();

	return true;
}

void ARAP::local()
{
	IGsize FaceNum = mesh->GetNumberOfFaces	();
	local_coord.resize(FaceNum, 6);
	
	for(igIndex fid = 0; fid < FaceNum; fid++)
	{
		igIndex pids[3];
		mesh->GetFacePointIds(fid, pids);
		for(int i=0;i<3;i++)
		{
			Eigen::Matrix2d J, P, S;
			P << UV(pids[(i+1)%3], 0) - UV(pids[i], 0), UV(pids[(i+2)%3], 0) - UV(pids[i], 0),
				UV(pids[(i+1)%3], 1) - UV(pids[i], 1), UV(pids[(i+2)%3], 1) - UV(pids[i], 1);
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
			Lts[i] = R;
		}
	}
}

void ARAP::global()
{
	IGsize nv = mesh->GetNumberOfPoints();
	IGsize nf = mesh->GetNumberOfFaces();
	Eigen::VectorXd bu, bv;
	bu.setZero(nv);
	bv.setZero(nv);
	for(int fid=0;fid<nf;fid++)
	{
		igIndex pids[3];
		Point p[3];
		mesh->GetFacePointIds(fid, pids);
		for(int i=0;i<3;i++) p[i] = mesh->GetPoint(pids[i]);
		Eigen::Vector2d e0, e1, e2;

		e0 << local_coord(fid, 2), local_coord(fid, 3);
		e1 << local_coord(fid, 4) - local_coord(fid, 2),  local_coord(fid, 5) - local_coord(fid, 3);
		e2 << -local_coord(fid, 4), -local_coord(fid, 5);

		Vector3d vec0 = p[1] - p[0];
		Vector3d vec1 = p[2] - p[1];
		Vector3d vec2 = p[0] - p[2];

		double cot0_1_2 = (-vec0).dot(vec1) / (-vec0).cross(vec1).norm();
		double cot1_2_0 = (-vec1).dot(vec2) / (-vec1).cross(vec2).norm();
		double cot2_0_1 = (-vec2).dot(vec0) / (-vec2).cross(vec0).norm();

		Eigen::Vector2d b0 = cot1_2_0 * Lts[fid] * e0;
		bu[pids[1]] += b0[0];
		bv[pids[1]] += b0[1];
		bu[pids[0]] -= b0[0];
		bv[pids[0]] -= b0[1];

		Eigen::Vector2d b1 = cot2_0_1 * Lts[fid] * e1;
		bu[pids[2]] += b1[0];
		bv[pids[2]] += b1[1];
		bu[pids[1]] -= b1[0];
		bv[pids[1]] -= b1[1];

		Eigen::Vector2d b2 = cot1_2_0 * Lts[fid] * e2;
		bu[pids[0]] += b2[0];
		bv[pids[0]] += b2[1];
		bu[pids[2]] -= b2[0];
		bv[pids[2]] -= b2[1];
	}
	UV.col(0) = global_solver.solve(bu);
	UV.col(1) = global_solver.solve(bv);
}

void ARAP::Initial()
{
	// Tutte paramization to get (u,v)
	IGsize FaceNum = mesh->GetNumberOfFaces();
	IGsize vnum = mesh->GetNumberOfPoints();
	std::vector<Eigen::Triplet<double>> coefficients;
	Eigen::SparseMatrix<double> mat(vnum, vnum);
	Eigen::VectorXd bx(vnum);
	Eigen::VectorXd by(vnum);
	std::vector<igIndex> boundary_vid;
	Lts.resize(FaceNum);
	UV.resize(vnum, 2);
	
	
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

	float circle_radian_speed = (2 * 3.14159265359) / boundary_vid.size();
	std::vector<bool> isIncluded;
	isIncluded.resize(boundary_vid.size(), false);
	int step = 0;
	int cur = 0;
	while(step < boundary_vid.size())
	{
		isIncluded[cur] = true;
		bx(boundary_vid[cur]) = std::cos(step * circle_radian_speed);
		by(boundary_vid[cur]) = std::sin(step * circle_radian_speed);
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
	Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
	solver.compute(mat);
	assert(solver.info() == Eigen::Success);
	UV.col(0) = solver.solve(bx);
	UV.col(1) = solver.solve(by);
	assert(solver.info() == Eigen::Success);

	//Construct the coeffitients matix of global step and the local coordinates of every 3d triangle. 
	std::vector<Eigen::Triplet<double>> global_coeff;
	Eigen::SparseMatrix<double> global_coeff_mat(vnum, vnum);
	local_coord.resize(FaceNum, 6);
	for(igIndex fid = 0; fid < FaceNum; fid ++)
	{
		igIndex pids[3];
		Point p[3];
		mesh->GetFacePointIds(fid, pids);
		for(int i=0;i<3;i++)
			p[i] = mesh->GetPoint(pids[i]);

		for(int i=0;i<3;i++)
		{
			auto eid = mesh->GetEdgeIdFormPointIds(pids[i], pids[(i+1)%3]);
			if(mesh->IsBoundaryEdge(eid)) continue;
			auto vec1 = p[(i+1)%3] - p[(i+2)%3];
			auto vec2 = p[i] - p[(i+2)%3];
			double cot = vec1.dot(vec2) / vec1.cross(vec2).norm();
			global_coeff.emplace_back(pids[i], pids[i], cot);
			global_coeff.emplace_back(pids[(i+1)%3], pids[(i+1)%3], cot);
			global_coeff.emplace_back(pids[i], pids[(i+1)%3], -cot);
			global_coeff.emplace_back(pids[(i+1)%3], pids[i], -cot);
		}

		Vector3f v0_v1 = p[1] - p[0];
		Vector3f v0_v2 = p[2] - p[0];
		Vector3f normal = mesh->GetFace(fid)->GetNormal();
		Vector3f v0_v1_norm = v0_v1.normalized();
		Vector3f identity_h = normal.cross(v0_v1_norm);
		local_coord.row(fid) << 0, 0, v0_v1.norm(), 0, v0_v2.dot(v0_v1_norm), v0_v2.dot(identity_h);
	}

	global_coeff_mat.setFromTriplets(global_coeff.begin(), global_coeff.end());
	global_solver.compute(global_coeff_mat);
}



IGAME_NAMESPACE_END