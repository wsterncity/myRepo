#ifndef iGameFace_h
#define iGameFace_h

#include "iGameCell.h"
#include "iGameLine.h"

IGAME_NAMESPACE_BEGIN
class Face : public Cell {
public:
	I_OBJECT(Face);

	virtual Vector3f GetNormal() = 0;

	int GetNumberOfFaces() override { return 0; }

	Cell* GetEdge(int edgeId) override 
	{
		int edgeIdPlus = (edgeId + 1) % this->GetNumberOfEdges();

		// load point id's
		this->Line->PointIds->SetId(0, this->PointIds->GetId(edgeId));
		this->Line->PointIds->SetId(1, this->PointIds->GetId(edgeIdPlus));

		// load coordinates
		this->Line->Points->SetPoint(0, this->Points->GetPoint(edgeId));
		this->Line->Points->SetPoint(1, this->Points->GetPoint(edgeIdPlus));

		return this->Line;
	}
	Cell* GetFace(int) override { return nullptr; }

protected:
	Face()
	{
		this->Line = Line::New();
	}
	~Face() override = default;

	Line::Pointer Line;
};
IGAME_NAMESPACE_END
#endif