#ifndef OSP_H
#define OSP_H
#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include <queue>
#include <random>

IGAME_NAMESPACE_BEGIN
class OSP : public Filter
{
public:
	I_OBJECT(OSP);
	static Pointer New(){return new OSP;}

	bool Execute() override
	{
		mesh = DynamicCast<SurfaceMesh>(GetInput(0));
		mesh->RequestEditStatus();
		fid2Region.resize(mesh->GetNumberOfFaces(), -1);
		fid2Area.resize(mesh->GetNumberOfFaces());
		region2ProxyNormal.resize(regionSize);
		region2Seed.resize(regionSize);

		for(int i = 0; i < 10; i++)
		{
			InitialSeeding();
			
			
			if (i > 0) 
			{
				assert(totalEnergy <= lastEnergy && "Optimal Error.");
				std::cout << "Iteration: " << i << " Total energy : " << totalEnergy << "\n";
				if (lastEnergy - totalEnergy < 1e-7)
				{
					std::cout << "Energy convergence." << std::endl;
					break;
				}
			}

			DistortionMinimizingFlooding();
			ProxyFitting();
		}
		DrawRegion();

		return true;
	}

	void SetPainter(Painter::Pointer p)
	{
		painter = p;
		painter->SetPen(Red);
		painter->SetPen(1);
	}

protected:
	OSP()
	{
		SetNumberOfInputs(1);
		SetNumberOfOutputs(0);
	}
	~ OSP() noexcept override = default;

	struct Item {
		double _priority;
		igIndex _fid;
		int	_label;

		explicit Item(double priority, igIndex fid, int label) : _priority(priority), _fid(fid), _label(label) {} ;
		bool operator<(const Item& other) const {
			return _priority > other._priority;
		}
	};

	Painter::Pointer painter;
	std::priority_queue<Item> queue;
	SurfaceMesh::Pointer mesh;
	std::vector<int> fid2Region;
	std::vector<double> fid2Area;
	std::vector<Vector3f> region2ProxyNormal;
	std::vector<igIndex> region2Seed;
	double totalEnergy = FLT_MAX;
	double lastEnergy = FLT_MAX;

	bool isInitial = true;
	
	int regionSize = 10;

	void InitialSeeding()
	{
		// At beginning picks k triangles at random to bootstrap algorithm.
		if(isInitial)
		{
			isInitial = false;

			// Calculate areas of every triangle and store them.
			for(igIndex fid=0; fid<mesh->GetNumberOfFaces(); fid++)
			{
				igIndex fvids[3];
				mesh->GetFacePointIds(fid, fvids);
				Point p0 = mesh->GetPoint(fvids[0]);
				Point p1 = mesh->GetPoint(fvids[1]);
				Point p2 = mesh->GetPoint(fvids[2]);

				double area = 0.5 * ((p1 - p0).cross(p2 - p0)).length();
				//std::cout << "fid area:" << fid << " " << area << std::endl;
				fid2Area[fid] = area;
			}

			int min = 0;
			int max = mesh->GetNumberOfFaces();
			std::random_device seed;
			std::ranlux48 engine(seed());
			std::uniform_int_distribution<> distrib(min, max);
			for (int region = 0; region < regionSize; region++)
			{
				igIndex fid = distrib(engine);
				region2Seed[region] = fid;
				region2ProxyNormal[region] = mesh->GetFace(fid)->GetNormal();
				fid2Region[fid] = region;
			}
		}

		// update seed triangles iteratively
		else
		{
			lastEnergy = totalEnergy;
			totalEnergy = 0;
			std::vector<double> region2minEnergy;
			region2minEnergy.resize(regionSize, FLT_MAX);

			for(igIndex fid=0;fid<mesh->GetNumberOfFaces();fid++)
			{
				int region = fid2Region[fid];
				double energy = CalcDistortionError(fid, region);
				totalEnergy += energy;
				if(energy < region2minEnergy[region])
				{
					region2Seed[region] = fid;
					region2minEnergy[region] = energy;
				}
			}

			for (int region = 0; region < regionSize; region++)
			{
				igIndex fid = region2Seed[region];
				fid2Region[fid] = region;
			}

		}
	}

	double CalcDistortionError(igIndex fid, int region)
	{
		Vector3f proxyNormal = region2ProxyNormal[region];
		Vector3f normal = mesh->GetFace(fid)->GetNormal();
		double energy = fid2Area[fid] * (normal - proxyNormal).squaredNorm();
		return energy;
	}

	void DistortionMinimizingFlooding()
	{
		std::vector<int> fid2label;
		fid2label.resize(mesh->GetNumberOfFaces(), -1);
		fid2Region.resize(mesh->GetNumberOfFaces(), -1);

		for(int region = 0; region < regionSize; region ++)
		{
			igIndex fid = region2Seed[region];
			igIndex ffids[3];
			fid2Region[fid] = region;
			mesh->GetFaceToNeighborFaces(fid, ffids);
			for(int i = 0; i < 3; i++)
				queue.push(Item(CalcDistortionError(ffids[i], region), ffids[i], region));
		}

		while(!queue.empty())
		{
			Item top = queue.top();queue.pop();
			if(fid2Region[top._fid] != -1) continue;
			else
			{
				fid2Region[top._fid] = top._label;
				igIndex ffids[64];
				int ffnum = mesh->GetFaceToNeighborFaces(top._fid, ffids);
				for(int i=0;i<ffnum;i++)
				{
					igIndex fid = ffids[i];
					if(fid2label[fid] != top._label && fid2Region[fid] == -1)
						queue.push(Item(CalcDistortionError(fid, top._label), fid, top._label));
				}
			}
		}


	}
	
	void ProxyFitting()
	{
		std::vector<Vector3f> region2ProxyNormalTemp;
		region2ProxyNormalTemp.resize(regionSize, Vector3f(0,0,0));
		IGsize faceNum = mesh->GetNumberOfFaces();
		for(igIndex fid = 0; fid < faceNum; fid ++)
		{
			int region = fid2Region[fid];
			Vector3f normal = mesh->GetFace(fid)->GetNormal();
			region2ProxyNormalTemp[region] += normal * fid2Area[fid];
		}

		for(int region=0;region<regionSize;region++)
			region2ProxyNormalTemp[region].normalize();

		region2ProxyNormal.swap(region2ProxyNormalTemp);
	}

	void DrawRegion()
	{
		std::vector<Color> region2Color;
		region2Color.reserve(regionSize);
		for(int i=1;i<=regionSize;i++)
			region2Color.push_back(Color(i + 1));
		
		IGsize faceNum = mesh->GetNumberOfFaces();
		for(igIndex fid=0; fid<faceNum; fid++)
		{
			int region = fid2Region[fid];
			//if(region == -1) continue;
			assert(region != -1);
			Color color = region2Color[region];
			igIndex fvids[3];
			mesh->GetFacePointIds(fid, fvids);
			painter->SetBrush(color);
			painter->Show(painter->DrawTriangle(mesh->GetPoint(fvids[0]),
				mesh->GetPoint(fvids[1]),
				mesh->GetPoint(fvids[2])));
		}

	}

};

IGAME_NAMESPACE_END

#endif //OSP_H