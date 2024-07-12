#ifndef iGameLine_h
#define iGameLine_h

#include "iGameCell.h"

IGAME_NAMESPACE_BEGIN
class Line : public Cell {
public:
	I_OBJECT(Line);
	static Pointer New() { return new Line; }
	static constexpr int CELL_TYPE = IG_LINE;
	static constexpr int CELL_SIZE = 2;

	int GetCellType() override { return IG_LINE; }
	int GetCellSize() override { return 2; }
	int GetCellDimension() override { return 1; }
	int GetNumberOfEdges() override { return 0; }
	int GetNumberOfFaces() override { return 0; }
	Cell* GetEdge(int) override { return nullptr; }
	Cell* GetFace(int) override { return nullptr; }

protected:
	Line()
	{
		this->Points->SetNumberOfPoints(2);
		this->PointIds->SetNumberOfIds(2);
		for (int i = 0; i < 2; i++)
		{
			this->Points->SetPoint(i, 0.0, 0.0, 0.0);
			this->PointIds->SetId(i, 0);
		}
	}
	~Line() override = default;
};
IGAME_NAMESPACE_END
#endif