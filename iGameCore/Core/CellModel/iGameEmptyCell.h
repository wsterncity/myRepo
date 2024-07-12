#ifndef iGameEmptyCell_h
#define iGameEmptyCell_h

#include "iGameCell.h"

IGAME_NAMESPACE_BEGIN
class EmptyCell : public Cell {
public:
	I_OBJECT(EmptyCell);
	static Pointer New() {	return new EmptyCell; }
	static constexpr int CELL_TYPE = IG_EMPTY_CELL;
	static constexpr int CELL_SIZE = 0;

	int GetCellType() override { return IG_EMPTY_CELL; }
	int GetCellSize() override { return 0; }
	int GetCellDimension() override { return 0; }
	int GetNumberOfEdges() override { return 0; }
	int GetNumberOfFaces() override { return 0; }
	Cell* GetEdge(int) override { return nullptr; }
	Cell* GetFace(int) override { return nullptr; }

protected:
	EmptyCell() = default;
	~EmptyCell() override = default;
};
IGAME_NAMESPACE_END
#endif