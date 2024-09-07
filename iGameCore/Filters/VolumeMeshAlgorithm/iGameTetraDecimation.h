#ifndef iGameTetraDecimation_h
#define iGameTetraDecimation_h

#include "iGameFilter.h"
#include "iGameVolumeMesh.h"
#include "iGamePriorityQueue.h"

IGAME_NAMESPACE_BEGIN
class TetraDecimation : public Filter {
public:
	I_OBJECT(TetraDecimation);
	static Pointer New() { return new TetraDecimation; }

	bool Execute() override {
        UpdateProgress(0);
        mesh = DynamicCast<VolumeMesh>(GetInput(0));
        if (mesh == nullptr) { return false; }
        mesh->RequestEditStatus();

        optimalPos.resize(mesh->GetNumberOfFaces() * 100);

        for (int i = 0; i < mesh->GetNumberOfFaces(); i++)
        {
            this->InsertFaceToHeap(i);
        }

		int needEliminatedNum = mesh->GetNumberOfFaces() / 2;
		for (int totalEliminated = 0; totalEliminated < 10; totalEliminated++)
		{
			heap->update();
			if (heap->empty()) {
				break;
			}
			igIndex faceId = heap->top().handle;
			double pri = heap->top().priority;
			heap->pop();

			igIndex f[3];
			mesh->GetFacePointIds(faceId, f);

			//if (!mesh->IsCollapsableFace(faceId)) continue;

			igIndex ids[64]{}, faceIds[64]{};
			for (int i = 0; i < 3; i++) {
				int size = mesh->GetPointToNeighborFaces(f[i], ids);
				for (int j = 0; j < size; j++) {
					heap->remove(ids[j]);
				}
			}

			//for (int i = 0; i < size; i++) {
			//	igIndex nei_f[3];
			//	mesh->GetFacePointIds(ids[i], nei_f);
			//	int count = 0;
			//	for (int m = 0; m < 3; m++) {
			//		for (int n = 0; n < 3; n++) {
			//			if (f[m] == nei_f[n]) {
			//				count++;
			//				break;
			//			}
			//		}
			//	}
			//	if (count == 1) {
			//		faceIds[k++] = ids[i];
			//		heap->remove(ids[i]);
			//		this->InsertFaceToHeap(ids[i]);
			//	}
			//	else {
			//		heap->remove(ids[i]);
			//	}
			//}

			this->CollapseFace(faceId);
			
			int newId = mesh->GetNumberOfPoints() - 1;
			int size = mesh->GetPointToNeighborFaces(newId, ids);
			for (int i = 0; i < size; i++) {
				this->InsertFaceToHeap(ids[i]);
			}
		}
		std::cout << mesh->GetNumberOfPoints() << std::endl;
		std::cout << mesh->GetNumberOfEdges() << std::endl;
		std::cout << mesh->GetNumberOfFaces() << std::endl;
		std::cout << mesh->GetNumberOfVolumes() << std::endl << std::endl;
		mesh->GarbageCollection();
		std::cout << mesh->GetNumberOfPoints() << std::endl;
		std::cout << mesh->GetNumberOfEdges() << std::endl;
		std::cout << mesh->GetNumberOfFaces() << std::endl;
		std::cout << mesh->GetNumberOfVolumes() << std::endl;
        
        UpdateProgress(1);
        SetOutput(0, mesh);
        return true;
    }

protected:
    TetraDecimation()
	{
		SetNumberOfInputs(1);
		heap = PriorityQueue::New();
	}
	~TetraDecimation() override = default;

	void CollapseFace(igIndex faceId) {
		igIndex f[3], ids[64]{};
		std::set<igIndex> st;
		std::vector<Vector<igIndex, 4>> vec;
		mesh->GetFacePointIds(faceId, f);
		int newId = mesh->GetNumberOfPoints();
		for (int i = 0; i < 3; i++) {
			int size = mesh->GetPointToNeighborVolumes(f[i], ids);
			for (int j = 0; j < size; j++) {
				st.insert(ids[j]);
			}
		}
		for (auto id : st) {
			int size = mesh->GetVolumePointIds(id, ids);
			int count = 0;
			Vector<igIndex, 4> temp{ ids[0], ids[1], ids[2], ids[3] };
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 4; j++) {
					if (f[i] == ids[j]) {
						count++;
						temp[j] = newId;
						break;
					}
				}
			}
			if (count == 1) {
				vec.push_back(temp);
			}
		}


		mesh->DeletePoint(f[0]);
		mesh->DeletePoint(f[1]);
		mesh->DeletePoint(f[2]);
		
		mesh->AddPoint(optimalPos[faceId]);
		for (int i = 0; i < vec.size(); i++) {
			mesh->AddVolume(vec[i].pointer(), 4);
		}
	}

    void InsertFaceToHeap(igIndex faceId)
    {
        int type = EvaluateFace(faceId);

        double priority = 0.0;
        if (type == 0) { // 内部三角形
            priority = ComputePriority(faceId);
        }

        if (priority != 0.0) {
            heap->push(-priority, faceId);
        }
    }

    int EvaluateFace(igIndex faceId)
    {
        int type = 0;
        igIndex f[3];
        mesh->GetFacePointIds(faceId, f);
        type += mesh->IsOnBoundaryPoint(f[0]) ? 1 : 0;
        type += mesh->IsOnBoundaryPoint(f[1]) ? 1 : 0;
        type += mesh->IsOnBoundaryPoint(f[2]) ? 1 : 0;

        return type;
    }

	double ComputePriority(igIndex faceId)
	{
		igIndex f[3];
		mesh->GetFacePointIds(faceId, f);
		Point v0 = mesh->GetPoint(f[0]);
		Point v1 = mesh->GetPoint(f[1]);
		Point v2 = mesh->GetPoint(f[2]);

		igIndex ids[128]{}, faceIds[128]{};
		int k = 0;
		int size = mesh->GetFaceToOneRingFaces(faceId, ids);
		for (int i = 0; i < size; i++) {
			igIndex nei_f[3];
			mesh->GetFacePointIds(ids[i], nei_f);
			int count = 0;
			for (int m = 0; m < 3; m++) {
				for (int n = 0; n < 3; n++) {
					if (f[m] == nei_f[n]) {
						count++;
						break;
					}
				}
			}
			if (count == 1) {
				faceIds[k++] = ids[i];
			}
		}

		double origPri[128]{};
		for (int i = 0; i < k; i++) {
			origPri[i] = this->GetTriFormFactor(ids[i]);
		}

		Point newPos = (v0 + v1 + v2) / 3;
		optimalPos[faceId] = newPos;

		mesh->SetPoint(f[0], newPos);
		mesh->SetPoint(f[1], newPos);
		mesh->SetPoint(f[2], newPos);

		double priority = 0.0;
		for (int i = 0; i < k; i++) {
			priority = std::max(priority, std::abs(origPri[i] - this->GetTriFormFactor(ids[i])));
		}

		mesh->SetPoint(f[0], v0);
		mesh->SetPoint(f[1], v1);
		mesh->SetPoint(f[2], v2);

		return priority;
	}

	double GetTriFormFactor(igIndex faceId)
	{
		igIndex face[3]{};
		int size = mesh->GetFacePointIds(faceId, face);
		Point v0 = mesh->GetPoint(face[0]);
		Point v1 = mesh->GetPoint(face[1]);
		Point v2 = mesh->GetPoint(face[2]);

		double a = (v0 - v1).length();
		double b = (v0 - v2).length();
		double c = (v1 - v2).length();

		if (a == 0.0 || b == 0.0 || c == 0.0) {
			return 0.0;
		}

		double q = (a + b + c) / 2;

		return 4 * (q - a) * (q - b) * (q - c) / (a * b * c);
	}

	double QualityFace(igIndex faceId)
	{
		auto f = mesh->GetFace(faceId);
		Point v0 = f->GetPoint(0);
		Point v1 = f->GetPoint(1);
		Point v2 = f->GetPoint(2);

		Vector3f d10 = v1 - v0;
		Vector3f d20 = v2 - v0;
		Vector3f d12 = v1 - v2;

		Vector3f normal = CrossProduct(d10, d20);

		double a = normal.norm();
		if (a == 0) return 0;
		double b = std::max(d10.squaredNorm(), std::max(d20.squaredNorm(), d12.squaredNorm()));
		if (b == 0) return 0;
		return a / b;
	}

	Vector3d Normal(igIndex faceId)
	{
		auto f = mesh->GetFace(faceId);
		Point v0 = f->GetPoint(0);
		Point v1 = f->GetPoint(1);
		Point v2 = f->GetPoint(2);

		Vector3f d10 = v1 - v0;
		Vector3f d20 = v2 - v0;

		Vector3d normal = CrossProduct(d10, d20);
		return normal;
	}

	igIndex GetEdgeFromVert(igIndex vertId, igIndex faceId) {
		auto f = mesh->GetFace(faceId);
		int i = 0;
		
		return f->GetCellSize();
	}

    VolumeMesh::Pointer mesh;
	PriorityQueue::Pointer heap;
    std::vector<Vector3d> optimalPos;
};
IGAME_NAMESPACE_END
#endif