#ifndef iGameHexahedron_h
#define iGameHexahedron_h

#include "iGameVolume.h"

IGAME_NAMESPACE_BEGIN
class Hexahedron : public Volume {
public:
	I_OBJECT(Hexahedron);
	static Pointer New() { return new Hexahedron; }

	int GetCellDimension() { return 3; }

	/**
	 * 单元的顶点个数。
	 */
	static constexpr int NumberOfPoints = 8;

	/**
	 * 单元的边个数。
	 */
	static constexpr int NumberOfEdges = 12;

	/**
	 * 单元的面个数。
	 */
	static constexpr int NumberOfFaces = 6;

	/**
	 * 单元面最大的顶点个数。
	 */
	static constexpr int MaximumFaceSize = 4;

	/**
	 * 单元顶点最大的度。
	 */
	static constexpr int MaximumValence = 3;

	/***************** 所有Maximum + 1的数组最后一位数字表示个数 *****************/

	// 边的顶点序号
	static constexpr int edges[NumberOfEdges][2] = {
	  { 0, 1 }, // 0
	  { 1, 2 }, // 1
	  { 3, 2 }, // 2
	  { 0, 3 }, // 3
	  { 4, 5 }, // 4
	  { 5, 6 }, // 5
	  { 7, 6 }, // 6
	  { 4, 7 }, // 7
	  { 0, 4 }, // 8
	  { 1, 5 }, // 9
	  { 3, 7 }, // 10
	  { 2, 6 }, // 11
	};

	// 面的顶点序号
	static constexpr int faces[NumberOfFaces][MaximumFaceSize + 1] = {
	  { 0, 4, 7, 3, 4 }, // 0
	  { 1, 2, 6, 5, 4 }, // 1
	  { 0, 1, 5, 4, 4 }, // 2
	  { 3, 7, 6, 2, 4 }, // 3
	  { 0, 3, 2, 1, 4 }, // 4
	  { 4, 5, 6, 7, 4 }, // 5
	};

	// 边的邻接面序号
	static constexpr int edgeToNeighborFaces[NumberOfEdges][2] = {
	  { 2, 4 }, // 0
	  { 1, 4 }, // 1
	  { 3, 4 }, // 2
	  { 0, 4 }, // 3
	  { 2, 5 }, // 4
	  { 1, 5 }, // 5
	  { 3, 5 }, // 6
	  { 0, 5 }, // 7
	  { 0, 2 }, // 8
	  { 1, 2 }, // 9
	  { 0, 3 }, // 10
	  { 1, 3 }, // 11
	};

	// 面的邻接面序号
	static constexpr int faceToNeighborFaces[NumberOfFaces][MaximumFaceSize + 1] = {
		{ 4, 2, 5, 3, 4 }, // 0
		{ 4, 3, 5, 2, 4 }, // 1
		{ 4, 1, 5, 0, 4 }, // 2
		{ 0, 5, 1, 4, 4 }, // 3
		{ 0, 3, 1, 2, 4 }, // 4
		{ 2, 1, 0, 3, 4 }, // 5
	};

	// 顶点的邻接边序号
	static constexpr int pointToNeighborEdges[NumberOfPoints][MaximumValence + 1] = {
		{ 0, 8,  3,  3 }, // 0
		{ 0, 1,  9,  3 }, // 1
		{ 1, 2,  11, 3 }, // 2
		{ 2, 3,  10, 3 }, // 3
		{ 7, 8,  4,  3 }, // 4
		{ 4, 9,  5,  3 }, // 5
		{ 5, 11, 6,  3 }, // 6
		{ 6, 10, 7,  3 }, // 7
	};

	// 顶点的邻接面序号
	static constexpr int pointToNeighborFaces[NumberOfPoints][MaximumValence + 1] = {
		{ 2, 0, 4, 3 }, // 0
		{ 4, 1, 2, 3 }, // 1
		{ 4, 3, 1, 3 }, // 2
		{ 4, 0, 3, 3 }, // 3
		{ 5, 2, 0, 3 }, // 4
		{ 2, 1, 5, 3 }, // 5
		{ 1, 3, 5, 3 }, // 6
		{ 3, 0, 5, 3 }, // 7
	};

	// 顶点的一邻域顶点序号
	static constexpr int pointToOneRingPoints[NumberOfPoints][MaximumValence + 1] = {
		{ 1, 4, 3, 3 }, // 0
		{ 0, 2, 5, 3 }, // 1
		{ 1, 3, 6, 3 }, // 2
		{ 2, 0, 7, 3 }, // 3
		{ 5, 7, 0, 3 }, // 4
		{ 4, 1, 6, 3 }, // 5
		{ 5, 2, 7, 3 }, // 6
		{ 6, 3, 4, 3 }, // 7
	};

	void GetEdgePoints(igIndex edgeId, const igIndex*& pts) override {
		assert(edgeId < NumberOfEdges && "edgeId too large");
		pts = edges[edgeId];
	}
	igIndex GetFacePoints(igIndex faceId, const igIndex*& pts) override {
		assert(faceId < NumberOfFaces && "faceId too large");
		pts = faces[faceId];
		return faces[faceId][MaximumFaceSize];
	}
	void GetEdgeToNeighborFaces(igIndex edgeId, const igIndex*& pts) override {
		assert(edgeId < NumberOfEdges && "edgeId too large");
		pts = edgeToNeighborFaces[edgeId];
	}
	igIndex GetFaceToNeighborFaces(igIndex faceId, const igIndex*& faceIds) override {
		assert(faceId < NumberOfFaces && "faceId too large");
		faceIds = faceToNeighborFaces[faceId];
		return faceToNeighborFaces[faceId][MaximumFaceSize];
	}
	igIndex GetPointToNeighborEdges(igIndex pointId, const igIndex*& edgeIds) override {
		assert(pointId < NumberOfPoints && "pointId too large");
		edgeIds = pointToNeighborEdges[pointId];
		return pointToNeighborEdges[pointId][MaximumValence];
	}
	igIndex GetPointToNeighborFaces(igIndex pointId, const igIndex*& faceIds) override {
		assert(pointId < NumberOfPoints && "pointId too large");
		faceIds = pointToNeighborFaces[pointId];
		return pointToNeighborFaces[pointId][MaximumValence];
	}
	igIndex GetPointToOneRingPoints(igIndex pointId, const igIndex*& pts) override {
		assert(pointId < NumberOfPoints && "pointId too large");
		pts = pointToOneRingPoints[pointId];
		return pointToOneRingPoints[pointId][MaximumValence];
	}

	int GetCellType() override { return IG_HEXAHEDRON; }
	int GetCellSize() override { return 8; }
	int GetNumberOfEdges() override { return 12; }
	int GetNumberOfFaces() override { return 6; }
	Cell* GetEdge(int edgeId) override
	{
		const int* verts = edges[edgeId];

		// load point id's
		this->Line->PointIds->SetId(0, this->PointIds->GetId(verts[0]));
		this->Line->PointIds->SetId(1, this->PointIds->GetId(verts[1]));

		// load coordinates
		this->Line->Points->SetPoint(0, this->Points->GetPoint(verts[0]));
		this->Line->Points->SetPoint(1, this->Points->GetPoint(verts[1]));

		return this->Line.get();
	}
	Cell* GetFace(int faceId) override
	{
		const int* verts = faces[faceId];
		for (int i = 0; i < 4; ++i)
		{
			this->Quad->PointIds->SetId(i, this->PointIds->GetId(verts[i]));
			this->Quad->Points->SetPoint(i, this->Points->GetPoint(verts[i]));
		}

		return this->Quad.get();
	}

private:
	Hexahedron()
	{
		this->Points->SetNumberOfPoints(4);
		this->PointIds->SetNumberOfIds(4);
		for (int i = 0; i < 4; i++)
		{
			this->Points->SetPoint(i, 0.0, 0.0, 0.0);
			this->PointIds->SetId(i, 0);
		}

		this->Line = Line::New();
		this->Quad = Quad::New();
	}
	~Hexahedron() override = default;

	Line::Pointer Line;
	Quad::Pointer Quad;
};


IGAME_NAMESPACE_END
#endif