#ifndef ODT_H
#define ODT_H
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
IGAME_NAMESPACE_BEGIN
class ODT : public Filter
{
public:
	I_OBJECT(ODT);
	static Pointer New() { return new ODT; }
	bool Execute() override 
	{
		mesh = DynamicCast<SurfaceMesh>(GetInput(0));
		for(int i=0;i<1000;i++)
		{
			mesh->RequestEditStatus();
			FlipEdge();
			UpdateVertices();
			mesh->GarbageCollection();
		}
		//mesh->RequestEditStatus();
		//FlipEdge();
		//UpdateVertices();
		//mesh->GarbageCollection();
		return true;
	}

	void SetPainter(Painter::Pointer p)
	{
		painter = p;
		painter->SetPen(Red);
		painter->SetPen(1);
	}

protected:

	SurfaceMesh::Pointer mesh;
	Painter::Pointer painter;
	std::vector<float> triangleArea;
	ODT() {
		SetNumberOfInputs(1);
		SetNumberOfOutputs(0);
	}

	~ODT() noexcept override = default;
	
	Point GetCircumCenter(igIndex fid)
	{
		igIndex fvids[3];
		mesh->GetFacePointIds(fid, fvids);
		Vector3f v0 = mesh->GetPoint(fvids[0]);
		Vector3f v1 = mesh->GetPoint(fvids[1]);
		Vector3f v2 = mesh->GetPoint(fvids[2]);
		
		float x1, y1, x2, y2, x3, y3;
		x1 = v0[0]; y1 = v0[1];
		x2 = v1[0]; y2 = v1[1];
		x3 = v2[0]; y3 = v2[1];

		float a1, b1, c1, a2, b2, c2;
		a1 = 2 * (x2 - x1);	  a2 = 2 * (x3 - x2);	c1 = x2 * x2 + y2 * y2 - x1 * x1 - y1 * y1;
		b1 = 2 * (y2 - y1);	  b2 = 2 * (y3 - y2);	c2 = x3 * x3 + y3 * y3 - x2 * x2 - y2 * y2;

		Vector3f circumcenter(0.0, 0.0, 0.0);
		circumcenter[0] = (b2 * c1 - b1 * c2) / (a1 * b2 - a2 * b1);
		circumcenter[1] = (a1 * c2 - a2 * c1) / (a1 * b2 - a2 * b1);
		circumcenter[2] = 0;

		return circumcenter;
	}
	float GetArea(igIndex fid)
	{
		igIndex fvids[3];
		mesh->GetFacePointIds(fid, fvids);
		Vector3f a = mesh->GetPoint(fvids[0]);
		Vector3f b = mesh->GetPoint(fvids[1]);
		Vector3f c = mesh->GetPoint(fvids[2]);
		return CrossProduct(b - a, c - a).norm() / 2;
	}
	void FlipEdge()
	{
		IGsize eNum = mesh->GetNumberOfEdges();
		for(int ei = 0; ei < eNum; ei ++)
		{
			if(mesh->IsBoundaryEdge(ei)) continue;
			igIndex eFids[64];
			igIndex eVids[2];
			igIndex counterVids[2];
			int eFNum = mesh->GetEdgeToNeighborFaces(ei, eFids);
			mesh->GetEdgePointIds(ei, eVids);
			//assert(eFNum == 2);
			for(int i=0;i<2;i++)
			{
				igIndex fVids[3];
				mesh->GetFacePointIds(eFids[i], fVids);
				counterVids[i] = fVids[0] + fVids[1] + fVids[2] - eVids[0] - eVids[1];
			}

			Point v1 = mesh->GetPoint(eVids[0]);
			Point v2 = mesh->GetPoint(eVids[1]);
			Point v3 = mesh->GetPoint(counterVids[0]);
			Point v4 = mesh->GetPoint(counterVids[1]);

			Vector3f v3_1 = v1 - v3;
			Vector3f v3_2 = v2 - v3;
			Vector3f v4_1 = v1 - v4;
			Vector3f v4_2 = v2 - v4;

			float angle3 = acos((pow((v1 - v3).norm(), 2) + pow((v2 - v3).norm(), 2) 
				- pow((v1 - v2).norm(), 2)) / (2 * (v1 - v3).norm() * (v2 - v3).norm()));
			float angle4 =  acos((pow((v1 - v4).norm(), 2) + pow((v2 - v4).norm(), 2) 
				- pow((v1 - v2).norm(), 2)) / (2 * (v1 - v4).norm() * (v2 - v4).norm()));

			if(angle3 + angle4 > (float)M_PI)
			{
				igIndex newFids[3];
				newFids[0] = counterVids[0];
				newFids[1] = counterVids[1];
				newFids[2] = eVids[0];
				mesh->AddFace(newFids, 3);
				newFids[2] = eVids[1];
				mesh->AddFace(newFids, 3);
				mesh->DeleteEdge(ei);
			}
		}
	}
	
	void UpdateVertices()
	{
		IGsize vNum = mesh->GetNumberOfPoints();
		painter->SetPen(Red);

		for(igIndex vid = 0; vid < vNum; vid ++)
		{
			if(mesh->IsBoundaryPoint(vid)) continue;
			igIndex vFids[64];
			int adjFNum = mesh->GetPointToNeighborFaces(vid, vFids);
			float LAR = 0;
			Point target(0,0,0);
			for(int i = 0; i < adjFNum; i++)
			{
				float area = GetArea(vFids[i]);
				LAR += area;
				target += area * GetCircumCenter(vFids[i]);
			}
			target = target / LAR;
			mesh->SetPoint(vid, target);
		}
	}

};

IGAME_NAMESPACE_END


#endif