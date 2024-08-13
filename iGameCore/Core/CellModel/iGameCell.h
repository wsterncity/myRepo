#ifndef iGameCell_h
#define iGameCell_h

#include "iGameObject.h"
#include "iGamePoints.h"
#include "iGameIdArray.h"
#include "iGameCellType.h"
#define IGAME_CELL_MAX_SIZE 256
IGAME_NAMESPACE_BEGIN
class Cell : public Object {
public:
	I_OBJECT(Cell);

	virtual IGenum GetCellType() const noexcept = 0;
	virtual int GetCellSize() const noexcept { return this->PointIds->GetNumberOfIds(); }

	virtual int GetNumberOfPoints() { return this->PointIds->GetNumberOfIds(); }
	virtual int GetNumberOfEdges() = 0;
	virtual int GetNumberOfFaces() = 0;

	igIndex GetPointId(const int ptId) { return this->PointIds->GetId(ptId); }
    Point& GetPoint(const int ptId) { return this->Points->GetPoint(ptId); }
    const Point& GetPoint(const int ptId) const {
		return this->Points->GetPoint(ptId);
    }

	virtual Cell* GetEdge(const int edgeId) = 0;
    virtual Cell* GetFace(const int faceId) = 0;

    void Reset() {
        this->PointIds->Reset();
        this->Points->Reset();
    }

	Points::Pointer Points{};
	IdArray::Pointer PointIds{};

protected:
	Cell() 
	{
		this->Points = Points::New();
		this->PointIds = IdArray::New();
	};
	~Cell() override = default;
};
IGAME_NAMESPACE_END
#endif