#ifndef iGameVertex_h
#define iGameVertex_h

#include "iGameCell.h"

IGAME_NAMESPACE_BEGIN
class Vertex : public Cell {
public:
	I_OBJECT(Vertex);
	static Pointer New() { return new Vertex; }

	IGenum GetCellType() const noexcept override { return IG_VERTEX; }
    int GetCellSize() const noexcept override { return 1; }
    int GetNumberOfEdges() override { return 0; }
    int GetNumberOfFaces() override { return 0; }
	Cell* GetEdge(const int) override { return nullptr; }
    Cell* GetFace(const int) override { return nullptr; }

protected:
	Vertex()
	{
		this->Points->SetNumberOfPoints(1);
		this->PointIds->SetNumberOfIds(1);
		for (int i = 0; i < 1; i++)
		{
			this->Points->SetPoint(i, 0.0, 0.0, 0.0);
			this->PointIds->SetId(i, 0);
		}
	}
	~Vertex() override = default;
};
IGAME_NAMESPACE_END
#endif