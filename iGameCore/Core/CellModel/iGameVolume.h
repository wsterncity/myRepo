#ifndef iGameVolume_h
#define iGameVolume_h

#include "iGameCell.h"
#include "iGameLine.h"
#include "iGameTriangle.h"
#include "iGameQuad.h"
#include "iGamePolygon.h"

IGAME_NAMESPACE_BEGIN
class Volume : public Cell {
public:
	I_OBJECT(Volume);

	virtual void GetEdgePoints(igIndex edgeId, const igIndex*& pts) = 0;
	virtual igIndex GetFacePoints(igIndex faceId, const igIndex*& pts) = 0;
	virtual void GetEdgeToNeighborFaces(igIndex edgeId, const igIndex*& pts) = 0;
	virtual igIndex GetFaceToNeighborFaces(igIndex faceId, const igIndex*& faceIds) = 0;
	virtual igIndex GetPointToNeighborEdges(igIndex pointId, const igIndex*& edgeIds) = 0;
	virtual igIndex GetPointToNeighborFaces(igIndex pointId, const igIndex*& faceIds) = 0;
	virtual igIndex GetPointToOneRingPoints(igIndex pointId, const igIndex*& pts) = 0;

protected:
	Volume() = default;
	~Volume() override = default;
};
IGAME_NAMESPACE_END
#endif