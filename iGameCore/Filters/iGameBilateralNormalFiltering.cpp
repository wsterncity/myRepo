#include "iGameBilateralNormalFiltering.h"

IGAME_NAMESPACE_BEGIN
bool BilateralNormalFiltering::Execute()
{
	mesh = DynamicCast<SurfaceMesh>(GetInput(0));
	mesh->RequestEditStatus();
	InitCache();
	for(int i=0;i<n_iter;i++)
		UpdateFaceNormal();
	for(int i=0;i<c_iter;i++)
		UpdateVerticePosition();
	return true;
}

void BilateralNormalFiltering::InitCache()
{
	sigema_c = 0;
	IGsize fnum = mesh->GetNumberOfFaces();
	newNormal_f.resize(fnum, Vector3f{0, 0, 0});
	center_f.resize(fnum, Point{0,0,0});
	area_f.resize(fnum, 0);
	for(IGsize fi = 0; fi < fnum; fi++)
	{
		auto face = mesh->GetFace(fi);
		newNormal_f[fi] = face->GetNormal();
		center_f[fi] = GetFaceCenter(fi);
		area_f[fi] = GetFaceArea(fi);
	}

	for(igIndex fi=0; fi<fnum; fi++)
	{
		igIndex adj_fids[64];
		IGsize adj_fnum = mesh->GetFaceToNeighborFaces(fi, adj_fids);
		for(int i = 0; i < adj_fnum; i++)
			sigema_c += (center_f[fi] - center_f[adj_fids[i]]).norm();
	}

	sigema_c /= (3 * fnum);
}

void BilateralNormalFiltering::UpdateFaceNormal()
{
	IGsize fnum = mesh->GetNumberOfFaces();
	float kp;
	Vector3f N;
	for(igIndex fi=0;fi<fnum;fi++)
	{
		kp = 0;
		N = Vector3f(0,0,0);
		igIndex AdjFids[64];
		IGsize adj_num = mesh->GetFaceToNeighborFaces(fi, AdjFids);
		for(int i=0;i<adj_num;i++)
		{
			float d_c = (center_f[fi] - center_f[AdjFids[i]]).norm();
			float d_n = (newNormal_f[fi] - newNormal_f[AdjFids[i]]).norm();
			float A = area_f[AdjFids[i]];
			float W_s = std::exp(-(d_c * d_c) / (2 * sigema_c * sigema_c));
			float W_r = std::exp(-(d_n * d_n) / (2 * sigema_r * sigema_r));
			kp += A * W_s * W_r;
			N += A * W_s * W_r * newNormal_f[AdjFids[i]];
		}
		newNormal_f[fi] = (N / kp).normalized();
	}
}

void BilateralNormalFiltering::UpdateVerticePosition()
{
	IGsize vnum = mesh->GetNumberOfPoints();
	for(igIndex vi = 0; vi < vnum; vi ++)
	{
		Vector3f dx{0,0,0};
		igIndex adj_fids[64];
		IGsize fnum = mesh->GetPointToNeighborFaces(vi, adj_fids);
		Point pos = mesh->GetPoint(vi);
		//float sum_area = 0;
		for(int i=0;i<fnum;i++)
		{
			Point center = GetFaceCenter(adj_fids[i]);
			//sum_area += GetFaceArea(adj_fids[i]);
			Vector3f N = newNormal_f[adj_fids[i]];
			dx += N * (N.dot(center - pos));
		}
		pos += dx / 18.0;
		//std::cout << "Update vertice " << vi << ": " << mesh->GetPoint(vi) << " to " << pos << std::endl;
		mesh->SetPoint(vi, pos);
		mesh->Modified();
	}
}

Point BilateralNormalFiltering::GetFaceCenter(igIndex fid)
{
	igIndex pids[3];
	Point p[3];
	mesh->GetFacePointIds(fid, pids);
	for(int i=0;i<3;i++)
		p[i] = mesh->GetPoint(pids[i]);
	return (p[0] + p[1] + p[2]) / 3;
}

float BilateralNormalFiltering::GetFaceArea(igIndex fid)
{
	igIndex pids[3];
	Point p[3];
	mesh->GetFacePointIds(fid, pids);
	for(int i=0;i<3;i++)
		p[i] = mesh->GetPoint(pids[i]);
	Vector3f v0_1 = p[1] - p[0];
	Vector3f v0_2 = p[2] - p[0];
	return v0_1.cross(v0_2).norm()/2;
}
IGAME_NAMESPACE_END