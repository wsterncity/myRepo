#ifndef iGameCell_h
#define iGameCell_h

#include "iGameObject.h"
#include "iGamePoints.h"
#include "iGameElementArray.h"
#include "iGameCellType.h"

IGAME_NAMESPACE_BEGIN
class Cell : public Object {
public:
	I_OBJECT(Cell);

	void Reset() {
		this->PointIds->Reset();
		this->Points->Reset();
	}

	void Initialize(int npts, const igIndex* pts) {
		this->PointIds->Reset();
		this->Points->Reset();

		for (int i = 0; i < npts; i++)
		{
			this->PointIds->AddId(pts[i]);
		}
	}

	virtual int GetCellType() = 0;

	virtual int GetCellSize() { return this->PointIds->GetNumberOfIds(); }

	virtual int GetCellDimension() = 0;

	virtual int IsLinear() { return 1; }

	Points::Pointer GetPoints() { return this->Points; }
	IdArray::Pointer GetPointIds() { return this->PointIds; }

	int GetNumberOfPoints() { return this->PointIds->GetNumberOfIds(); }

	virtual int GetNumberOfEdges() = 0;

	virtual int GetNumberOfFaces() = 0;

	igIndex GetPointId(int ptId) { return this->PointIds->GetId(ptId); }
	Point& GetPoint(int ptId) { return this->Points->GetPoint(ptId); }
	const Point& GetPoint(int ptId) const { return this->Points->GetPoint(ptId); }

	virtual Cell* GetEdge(int edgeId) = 0;
	virtual Cell* GetFace(int faceId) = 0;

	//Box3d GetBounds() { return this->Bounds; }

	Points::Pointer Points{};
	IdArray::Pointer PointIds{};

protected:
	Cell() 
	{
		this->Points = Points::New();
		this->PointIds = IdArray::New();
	};
	~Cell() override = default;

	//Box3d Bounds;
};
IGAME_NAMESPACE_END
#endif