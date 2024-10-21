#include "iGameCurvature.h"
IGAME_NAMESPACE_BEGIN
bool Curvature::Execute()
{
	mesh = DynamicCast<SurfaceMesh>(GetInput(0));
	mesh->RequestEditStatus();
	InitCache();
	
	FloatArray::Pointer property = FloatArray::New();
	property->SetDimension(1);

	IGsize vnum = mesh->GetNumberOfPoints();
	if(mode == MeanCurvatureMode)
	{
		property->SetName("MeanCurvatureScalar");
		for(int i = 0; i < vnum; i++)
		{
			if(MeanCurvature_v[i].dot(Normal_v[i]) > 0)
				property->AddValue(-MeanCurvature_v[i].norm());
			else property->AddValue(MeanCurvature_v[i].norm());

		}
	}

	else if(mode == AbsoluteMeanCurvatureMode)
	{
		property->SetName("AbsoluteMeanCurvatureScalar");
		for(int i = 0; i < vnum; i++)
			property->AddValue(MeanCurvature_v[i].norm());
	}

	else if(mode == GuassianCurvatureMode)
	{
		property->SetName("GuassianCurvatureScalar");
		for(int i = 0; i < vnum; i++)
			property->AddValue(GuassianCurvature_v[i]);
	}

	mesh->GetAttributeSet()->AddScalar(IG_POINT, property);
	/*
	std::vector<float> P2C;
	P2C.resize(vnum);
	float MinV = INFINITY;
	float MaxV = -INFINITY;
	float curvature;
	for(igIndex pid=0;pid<vnum;pid++)
	{
		float val = MeanCurvature_v[pid].dot(Normal_v[pid]);
		if(val > 0) curvature = MeanCurvature_v[pid].norm();
		else curvature = -MeanCurvature_v[pid].norm();
		if(curvature < MinV) MinV = curvature;
		if(curvature > MaxV) MaxV = curvature;
		P2C[pid] = curvature;
	}

	for(igIndex pid=0;pid<vnum;pid++)
	{
		float rv = (P2C[pid] - MinV) / (MaxV - MinV);
	}
	*/	 
	//auto pro=property->RawPointer();
	//for(int i=0;i<vnum;i++){
	//	pro[i]=1.5*pro[i];
	//}
	return true;
}

Vector3f Curvature::CalcCircumCenter(const Vector3f& a, const Vector3f& b, const Vector3f& c)
{
	Vector3f ac = c - a, ab = b - a;
	Vector3f abXac = ab.cross(ac), abXacXab = abXac.cross(ab), acXabXac = ac.cross(abXac);
	return a + (abXacXab * ac.squaredNorm() + acXabXac * ab.squaredNorm()) / (2.0 * abXac.squaredNorm());
}

void Curvature::CalcLAR()
{
	IGsize fnum = mesh->GetNumberOfFaces();
	for(int fi=0; fi < fnum; fi ++)
	{
		igIndex pids[3];
		igIndex obtuseVertex;
		Point p[3];

		// judge if it's obtuse
		bool isObtuseAngle = false;
		mesh->GetFacePointIds(fi, pids);
		for(int i=0;i<3;i++)
			p[i] = mesh->GetPoint(pids[i]);
		
		if(CalcAngle(p[1], p[0], p[2]) > PI/2)
		{
			isObtuseAngle = true;
			obtuseVertex = pids[1];
		}
		else if(CalcAngle(p[0], p[1], p[2]) > PI/2)
		{
			isObtuseAngle = true;
			obtuseVertex = pids[1];
		}
		else if(CalcAngle(p[0], p[2], p[1]) > PI/2)
		{
			isObtuseAngle = true;
			obtuseVertex = pids[2];
		}

		if(isObtuseAngle)
		{
			float faceArea = 0.5 * ((p[1] - p[0]).cross(p[2] - p[0])).norm();
			for(int i=0;i<3;i++)
			{
				if(pids[i] == obtuseVertex) LAR_v[pids[i]] +=  faceArea / 2.0;
				else LAR_v[pids[i]] += faceArea / 4.0;
			}
		}
		else
		{
			Point center = CalcCircumCenter(p[0], p[1], p[2]);
			for(int i=0;i<3;i++)
			{
				Point mid1 = (p[i] + p[(i + 1)%3]) / 2;
				Point mid2 = (p[i] + p[(i + 2)%3]) / 2;
				LAR_v[pids[i]] += 0.5 * (center - mid1).norm() * (p[i] - p[(i + 1)%3]).norm();
				LAR_v[pids[i]] += 0.5 * (center - mid2).norm() * (p[i] - p[(i + 2)%3]).norm();
			}
		}
		
	}
}

void Curvature::CalcNormal()
{
	IGsize fnum = mesh->GetNumberOfFaces();
	for(int fi=0; fi < fnum; fi ++)
	{
		igIndex pids[3];
		mesh->GetFacePointIds(fi, pids);
		auto face = mesh->GetFace(fi);
		for(int i=0;i<3;i++)
			Normal_v[pids[i]] += face->GetNormal();
	}

	for(auto& n : Normal_v)
		n.normalize();
}

float Curvature::CalcAngle(const Vector3f& a, const Vector3f& b, const Vector3f& c)
{
	Vector3f ba = a - b, bc = c - b;
	float cos_abc = ba.dot(bc) / (ba.norm()*bc.norm());
	float angle_abc = std::acos(cos_abc);
	return angle_abc;
}

void Curvature::CalcGuassianCurvature()
{
	IGsize fnum = mesh->GetNumberOfFaces();
	IGsize pnum = mesh->GetNumberOfPoints();
	std::vector<float> angel;
	angel.resize(pnum, 0);
	for(int fi=0;fi<fnum;fi++)
	{
		igIndex pids[3];
		Point p[3];
		mesh->GetFacePointIds(fi, pids);
		for(int i=0;i<3;i++)
			p[i] = mesh->GetPoint(pids[i]);
		for(int i=0;i<3;i++)
			angel[pids[i]] += CalcAngle(p[(i+1)%3],p[i], p[(i+2)%3]);
	}
	
	for(int i=0;i<pnum;i++)
		GuassianCurvature_v[i] = (2 * PI - angel[i]) / (LAR_v[i]);
}

void Curvature::CalcMeanCurvature()
{
	IGsize fnum = mesh->GetNumberOfFaces();
	for(int fi = 0; fi < fnum; fi ++)
	{
		igIndex pids[3];
		Point p[3];
		mesh->GetFacePointIds(fi, pids);
		for(int i=0;i<3;i++)
			p[i] = mesh->GetPoint(pids[i]);
		float cot0_1_2 = 1 / std::tan(CalcAngle(p[0], p[1], p[2]));
		float cot1_0_2 = 1 / std::tan(CalcAngle(p[1], p[0], p[2]));
		float cot0_2_1 = 1 / std::tan(CalcAngle(p[0], p[2], p[1]));
		MeanCurvature_v[pids[0]] += ((p[1] - p[0]) * cot0_2_1 + (p[2] - p[0]) * cot0_1_2) /( 4 * LAR_v[pids[0]]);
		MeanCurvature_v[pids[1]] += ((p[0] - p[1]) * cot0_2_1 + (p[2] - p[1]) * cot1_0_2) /( 4 * LAR_v[pids[1]]);
		MeanCurvature_v[pids[2]] += ((p[0] - p[2]) * cot0_1_2 + (p[1] - p[2]) * cot1_0_2) / (4 * LAR_v[pids[2]]);
	}
}

void Curvature::InitCache()
{
	IGsize fnum = mesh->GetNumberOfFaces();
	IGsize pnum = mesh->GetNumberOfPoints();
	MeanCurvature_v.resize(pnum, Vector3f{0,0,0});
	GuassianCurvature_v.resize(pnum, 0);
	LAR_v.resize(pnum, 0);
	Normal_v.resize(pnum, Vector3f{0,0,0});
	CalcLAR();
	CalcNormal();
	if(mode == MeanCurvatureMode || mode == AbsoluteMeanCurvatureMode)
		CalcMeanCurvature();
	else if(mode == GuassianCurvatureMode)
		CalcGuassianCurvature();
}

IGAME_NAMESPACE_END