#ifndef iGamePolyLine_h
#define iGamePolyLine_h

#include "iGameCell.h"

IGAME_NAMESPACE_BEGIN
class PolyLine : public Cell {
public:
    I_OBJECT(PolyLine);
    static Pointer New() { return new PolyLine; }

	int GetCellType() override { return IG_POLY_LINE; }
	int GetCellDimension() override { return 1; }
	int GetNumberOfEdges() override { return 0; }
	int GetNumberOfFaces() override { return 0; }
	Cell* GetEdge(int) override { return nullptr; }
	Cell* GetFace(int) override { return nullptr; }

protected:
    PolyLine()
	{

	}
    ~PolyLine() override = default;
};
IGAME_NAMESPACE_END
#endif