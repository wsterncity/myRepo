#ifndef IncrementalRemeshing_h
#define IncrementalRemeshin_h
#include "iGameFilter.h"
#include "iGamePointSet.h"
#include "iGameSurfaceMesh.h"
#include "AABB_Tree.h"
IGAME_NAMESPACE_BEGIN
class IncrementalRemeshing : public Filter
{
public:
	I_OBJECT(IncrementalRemeshing);
	static Pointer New() { return new IncrementalRemeshing; }
	bool Execute() override 
	{
		mesh = DynamicCast<SurfaceMesh>(GetInput(0));
		get_AABB_tree();

		double targetEdgeLength = CalcEdgeLength();
		//double low = (4.0 / 5.0) * targetEdgeLength;
		//double high = (4.0 / 3.0) * targetEdgeLength;

		double low = 0.129789;
		double high = 0.216315;
		std::cout << "low, high: " << low << ", " << high << std::endl;
		for(int i = 0; i < 10; i ++)
		{
			mesh->RequestEditStatus();
			SplitLongEdges(high);
			CollapseShortEdges(low, high);
			EqualizeValences();
			TangentialRelaxation();
			ProjectionToSurface();
			mesh->GarbageCollection();
		}



		//mesh->RequestEditStatus();
		//switch (step)
		//{
		//case(0):
		//	SplitLongEdges(high);
		//	CollapseShortEdges(low, high);
		//	EqualizeValences();
		//	TangentialRelaxation();
		//	ProjectionToSurface();
		//	break;
		//case(1):SplitLongEdges(high);break;
		//case(2):CollapseShortEdges(low, high);break;
		//case(3):EqualizeValences();break;
		//case(4):TangentialRelaxation();break;
		//case(5):ProjectionToSurface();break;
		//default:
		//	break;
		//}
		//mesh->GarbageCollection();
		
		return true;
	}

	void SetStep(int s){step = s;}

	void SetDraw(Painter::Pointer p)
	{
		drawer = p;
		drawer->SetPen(2);
		drawer->SetPen(Red);
	}

protected:
	int step = -1;
	SurfaceMesh::Pointer mesh;
	AABB_Tree* abtree;
	Painter::Pointer drawer;
	IncrementalRemeshing()
	{
		SetNumberOfInputs(1);
		SetNumberOfOutputs(0);
	}

	~IncrementalRemeshing() noexcept override = default;

	double CalcEdgeLength()
	{
		IGsize edgeNum = mesh->GetNumberOfEdges();
		double targetEdgeLength = 0;
		for(igIndex eid = 0; eid < edgeNum; eid ++)
		{
			igIndex pids[2];
			mesh->GetEdgePointIds(eid, pids);
			targetEdgeLength += (mesh->GetPoint(pids[0]) - mesh->GetPoint(pids[1])).length();
		}

		return targetEdgeLength / edgeNum / 2.0;
	}

	void get_AABB_tree()
	{
		IGsize fnum = mesh->GetNumberOfFaces();
		std::vector<Vector3f> point_set;
		point_set.clear();
		for (igIndex fid = 0; fid < fnum; fid++)
		{
			igIndex fvids[3];
			mesh->GetFacePointIds(fid, fvids);
			for(int i=0;i<3;i++)
			{
				igIndex vid = fvids[i];
				point_set.push_back(mesh->GetPoint(vid));
			}

		}
		abtree = new AABB_Tree(point_set);
	}

	void SplitLongEdges(double high)
	{
		IGsize edgeNum = mesh->GetNumberOfEdges();
		for(igIndex eid = 0; eid < edgeNum; eid ++)
		{
			igIndex fids[2];
			igIndex vids[2];
			std::vector<igIndex> counterVid;
			Point ev[2], cv;
			int AdjFaceNum = mesh->GetEdgeToNeighborFaces(eid, fids);
			int AdjVerticeNum = mesh->GetEdgePointIds(eid, vids);
			assert((AdjFaceNum == 2 || AdjFaceNum == 1) && AdjVerticeNum == 2);
			ev[0] = mesh->GetPoint(vids[0]);
			ev[1] = mesh->GetPoint(vids[1]);
			if((ev[0]-ev[1]).length() < high) continue;

			cv = (ev[0] + ev[1]) / 2;
			igIndex cvid = mesh->AddPoint(cv);
			for(int i=0;i<AdjFaceNum;i++)
			{
				igIndex fvids[3], fvid;
				igIndex newFaceVids[3];

				mesh->GetFacePointIds(fids[i], fvids);
				fvid = (fvids[0] + fvids[1] + fvids[2]) - (vids[0] + vids[1]);
				newFaceVids[0] = vids[0];
				newFaceVids[1] = cvid;
				newFaceVids[2] = fvid;
				mesh->AddFace(newFaceVids, 3);
				newFaceVids[0] = vids[1];
				mesh->AddFace(newFaceVids, 3);
			}
			mesh->DeleteEdge(eid);
		}
	}


	void CollapseShortEdges(double low, double high)
	{
		IGsize edgeNum = mesh->GetNumberOfEdges();
		for(igIndex eid = 0; eid < edgeNum; eid ++)
		{
			if(!IsCollapseOk(eid, low, high)) continue;
			igIndex vids[2];
			igIndex fids[2];
			mesh->GetEdgePointIds(eid, vids);
			int AdjFaceNum = mesh->GetEdgeToNeighborFaces(eid, fids);

			igIndex survivalVid = vids[1];
			igIndex extinctVid = vids[0];

			igIndex OneRingFids[64];
			int facenum = mesh->GetPointToNeighborFaces(extinctVid, OneRingFids);
			for(int j=0; j<facenum; j++)
			{
				bool isFaceIgnore = false;

				for(int k = 0; k < AdjFaceNum; k++)
					if(fids[k] == OneRingFids[j]) isFaceIgnore = true;
				if(isFaceIgnore) continue;

				igIndex fvids[3], newFvids[3];
				mesh->GetFacePointIds(OneRingFids[j], fvids);
				newFvids[0] = survivalVid;
				int sameVidLoc = 0;
				while(fvids[sameVidLoc] != extinctVid) sameVidLoc++;
				assert(sameVidLoc < 3);
				newFvids[1] = fvids[(sameVidLoc + 1) % 3];
				newFvids[2] = fvids[(sameVidLoc + 2) % 3];
				mesh->AddFace(newFvids, 3);
			}
			mesh->DeletePoint(extinctVid);
		}
	}

	//The target valence is 6 and 4 for interior and boundary vertices.
	void EqualizeValences()
	{
		IGsize edgeNum = mesh->GetNumberOfEdges();
		IGsize vNum = mesh->GetNumberOfPoints();
		std::vector<int> target_valence;
		for(igIndex vid = 0; vid < vNum; vid++)
		{
			if (mesh->IsBoundaryPoint(vid)) target_valence.push_back(4);
			else target_valence.push_back(6);
		}
		for(igIndex eid = 0; eid < edgeNum; eid ++)
		{
			if(mesh->IsBoundaryEdge(eid)) continue;
			int deviationPre = 0;
			int deviationPost = 0;
			igIndex fids[2];
			igIndex vids[2];
			std::vector<int> CounterVid;
			
			int fNum = mesh->GetEdgeToNeighborFaces(eid, fids);
			int vNum = mesh->GetEdgePointIds(eid, vids);
			if( fNum > 2)
			{
				drawer->Show(drawer->DrawLine(mesh->GetPoint(vids[0]), mesh->GetPoint(vids[1])));
				std::cout << "error topology, point:" << mesh->GetPoint(vids[0]) << ", " << mesh->GetPoint(vids[1]) << std::endl;
				continue;
			}
			for(int i=0;i<fNum;i++)
			{
				igIndex fid = fids[i];
				igIndex fvids[3];
				igIndex edgeCounterVid;
				mesh->GetFacePointIds(fid, fvids);
				edgeCounterVid = (fvids[0] + fvids[1] + fvids[2]) - (vids[0] + vids[1]);
				CounterVid.push_back(edgeCounterVid);
			}
			deviationPre = abs(mesh->GetPointValence(vids[0]) - target_valence[vids[0]]) + abs(mesh->GetPointValence(vids[1]) - target_valence[vids[1]])
				+ abs(mesh->GetPointValence(CounterVid[0]) - target_valence[CounterVid[0]]) + abs(mesh->GetPointValence(CounterVid[1]) - target_valence[CounterVid[1]]);
			deviationPost = abs(mesh->GetPointValence(vids[0]) - 1 - target_valence[vids[0]]) + abs(mesh->GetPointValence(vids[1]) - 1 - target_valence[vids[1]])
				+ abs(mesh->GetPointValence(CounterVid[0]) + 1 - target_valence[CounterVid[0]]) + abs(mesh->GetPointValence(CounterVid[1]) + 1 - target_valence[CounterVid[1]]);

			if(deviationPre > deviationPost)
			{
				igIndex newFaceVids[3];
				newFaceVids[0] = CounterVid[0];
				newFaceVids[1] = CounterVid[1];
				newFaceVids[2] = vids[0];
				mesh->AddFace(newFaceVids, 3);
				newFaceVids[2] = vids[1];
				mesh->AddFace(newFaceVids, 3);
				mesh->DeleteEdge(eid);
			}
		}
	}

	void TangentialRelaxation()
	{
		IGsize vnum = mesh->GetNumberOfPoints();
		for(igIndex vid = 0; vid < vnum; vid ++)
		{
			if(mesh->IsBoundaryPoint(vid)) continue;
			igIndex adjVids[64];
			igIndex adjFids[64];
			Vector3f normal(0.0, 0.0, 0.0);
			Vector3f loc = mesh->GetPoint(vid);
			Vector3f locPost, laplaceLoc(0.0,0.0,0.0);

			int valence = mesh->GetPointToOneRingPoints(vid, adjVids);
			int facenum = mesh->GetPointToNeighborFaces(vid, adjFids);
			
			for(int i=0;i<facenum;i++)
			{
				igIndex fid = adjFids[i];
				normal += mesh->GetFace(fid)->GetNormal();
			}

			normal = (normal / facenum).normalized();

			for(int i=0;i<valence;i++)
			{
				igIndex vvid = adjVids[i];
				laplaceLoc += mesh->GetPoint(vvid);
			}

			laplaceLoc = laplaceLoc / valence;

			locPost = laplaceLoc + DotProduct(normal, loc - laplaceLoc) * normal;

			mesh->SetPoint(vid, locPost);
		}
	}


	void ProjectionToSurface()
	{
		IGsize vnum = mesh->GetNumberOfPoints();
		for(igIndex vid = 0; vid < vnum; vid++)
		{
			Point p = mesh->GetPoint(vid);
			Vector3f ab_nearst_point;
			abtree->findNearstPoint(p, ab_nearst_point);
			mesh->SetPoint(vid, ab_nearst_point);
		}
	}

	bool IsCollapseOk(igIndex eid, float low, float high)
	{
		if (mesh->IsEdgeDeleted(eid) || mesh->IsBoundaryEdge(eid)) return false;
		igIndex vids[2];
		igIndex fids[2];
		mesh->GetEdgePointIds(eid, vids);

		igIndex survivalVid = vids[1];
		igIndex extinctVid = vids[0];

		Point survivalPoint = mesh->GetPoint(vids[1]);
		Point extinctPoint = mesh->GetPoint(vids[0]);
		
		if(mesh->IsBoundaryPoint(survivalVid) || mesh->IsBoundaryPoint(extinctVid)) return false;
		if((extinctPoint - survivalPoint).length() > low) return false;

		igIndex eOneRingVids[64], sOneRingVids[64];
		int evvnum = mesh->GetPointToOneRingPoints(extinctVid, eOneRingVids);
		int svvnum = mesh->GetPointToOneRingPoints(survivalVid, sOneRingVids);
		for(int i=0;i<evvnum;i++)
			if((mesh->GetPoint(eOneRingVids[i]) - survivalPoint).length() > high)
				return false;
		
		//For triangular mesh, two vertices only share two common points
		int samePointNum = 0;
		for(int i = 0; i< evvnum; i++)
			for(int j = 0; j < svvnum; j++)
				if(eOneRingVids[i] == sOneRingVids[j]) samePointNum ++;
		if(samePointNum > 2) return false;


		
		int facenum = mesh->GetEdgeToNeighborFaces(eid, fids);
		
		//For a triangular face, the other two edge cannot both be boundary edges
		for(int i=0;i<facenum;i++)
		{
			igIndex feids[64];
			int feNum = mesh->GetFaceEdgeIds(fids[i], feids);
			assert(feNum == 3);
			int loc = 0;
			while(feids[loc] != eid) loc ++;
			if(mesh->IsBoundaryEdge(feids[(loc+1)%3]) && mesh->IsBoundaryEdge(feids[(loc+2)%3]))
				return false;
		}

		// edge between two boundary vertices should be a boundary edge
		if (mesh->IsBoundaryPoint(survivalVid) && mesh->IsBoundaryPoint(extinctVid) && !mesh->IsBoundaryEdge(mesh->GetEdgeIdFormPointIds(survivalVid, extinctVid)))
			return false;

		return true;
	}

};

IGAME_NAMESPACE_END

#endif