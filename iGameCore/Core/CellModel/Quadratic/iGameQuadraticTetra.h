#ifndef iGameQuadraticTetra_h
#define iGameQuadraticTetra_h

#include "Base/iGameQuadraticVolume.h"
#include "iGameQuadraticTriangle.h"
IGAME_NAMESPACE_BEGIN
class QuadraticTetra : public QuadraticVolume {
public:
	I_OBJECT(QuadraticTetra);
	static Pointer New() { return new QuadraticTetra; }

	IGenum GetCellType() const noexcept override { return IG_QUADRATIC_TETRA; }
	int GetCellSize() const noexcept override { return 10; }
	int GetNumberOfEdges() override { return 6; }
	int GetNumberOfFaces() override { return 4; }

	Cell* GetEdge(const int edgeId) override {
		const int* verts = edges[edgeId];

		m_Line->PointIds->SetId(0, this->PointIds->GetId(verts[0]));
		m_Line->PointIds->SetId(1, this->PointIds->GetId(verts[1]));
		m_Line->PointIds->SetId(2, this->PointIds->GetId(verts[2]));

		m_Line->Points->SetPoint(0, this->Points->GetPoint(verts[0]));
		m_Line->Points->SetPoint(1, this->Points->GetPoint(verts[1]));
		m_Line->Points->SetPoint(2, this->Points->GetPoint(verts[2]));

		return m_Line.get();
	}
	Cell* GetFace(const int faceId) override {
		const int* verts = faces[faceId];
		for (int i = 0; i < 3; ++i) {
			m_Triangle->PointIds->SetId(i, this->PointIds->GetId(verts[i]));
			m_Triangle->Points->SetPoint(i, this->Points->GetPoint(verts[i]));
		}

		return m_Triangle.get();
	}

	/**
	 * 单元的顶点个数。
	 */
	static constexpr int NumberOfPoints = 10;

	/**
	 * 单元的边个数。
	 */
	static constexpr int NumberOfEdges = 6;

	/**
	 * 单元的面个数。
	 */
	static constexpr int NumberOfFaces = 4;

	/**
	 * 单元面最大的顶点个数。
	 */
	static constexpr int MaxFaceSize = 6;

	/**
	 * 单元顶点最大的度。
	 */
	static constexpr int MaxValence = 3;

	/***************** 所有Max + 1的数组最后一位数字表示个数 *****************/

	// 边的顶点序号
	static constexpr int edges[NumberOfEdges][3] = {
	  { 0, 1, 4},
	  { 1, 2, 5},
	  { 2, 0, 6},
	  { 0, 3, 7},
	  { 1, 3, 8},
	  { 2, 3, 9},
	};

	// 面的顶点序号
	static constexpr int faces[NumberOfFaces][MaxFaceSize + 1] = {
	  { 0, 1, 3, 4, 8, 7, 6 },
	  { 1, 2, 3, 5, 9, 8, 6 },
	  { 2, 0, 3, 6, 7, 9, 6 },
	  { 0, 2, 1, 6, 5, 4, 6 },
	};

	// 面的边序号
	static constexpr int faceEdges[NumberOfFaces][MaxFaceSize / 2 + 1] = {
	  { 0, 4, 3, 3 },
	  { 1, 5, 4, 3 },
	  { 2, 3, 5, 3 },
	  { 2, 1, 0, 3 },
	};

	// 边的邻接面序号
	static constexpr int edgeToNeighborFaces[NumberOfEdges][2] = {
	  { 0, 3 },
	  { 1, 3 },
	  { 2, 3 },
	  { 0, 2 },
	  { 0, 1 },
	  { 1, 2 },
	};

	// 面的邻接面序号
	static constexpr int faceToNeighborFaces[NumberOfFaces][MaxFaceSize + 1] = {
	  { 3, 1, 2, 3 },
	  { 3, 2, 0, 3 },
	  { 3, 0, 1, 3 },
	  { 2, 1, 0, 3 },
	};

	// 顶点的邻接边序号
	static constexpr int pointToNeighborEdges[NumberOfPoints][MaxValence + 1] = {
	  { 0, 3, 2, 3 },
	  { 0, 1, 4, 3 },
	  { 1, 2, 5, 3 },
	  { 3, 4, 5, 3 },
	};

	// 顶点的邻接面序号
	static constexpr int pointToNeighborFaces[NumberOfPoints][MaxValence + 1] = {
	  { 0, 2, 3, 3 },
	  { 3, 1, 0, 3 },
	  { 3, 2, 1, 3 },
	  { 0, 1, 2, 3 },
	};

	// 顶点的一邻域顶点序号
	static constexpr int pointToOneRingPoints[NumberOfPoints][MaxValence + 1] = {
	  { 1, 3, 2, 3 },
	  { 0, 2, 3, 3 },
	  { 1, 0, 3, 3 },
	  { 0, 1, 2, 3 },
	};

	int GetEdgePointIds(const int edgeId, const igIndex*& ptIds) override {
		ptIds = edges[edgeId];
		return 3;
	}
	int GetFacePointIds(const int faceId, const igIndex*& ptIds) override {
		ptIds = faces[faceId];
		return faces[faceId][MaxFaceSize];
	}
	int GetFaceEdgeIds(const int faceId, const igIndex*& edgeIds) override {
		edgeIds = faceEdges[faceId];
		return faceEdges[faceId][MaxFaceSize];
	}

	int GetPointToOneRingPoints(const int pointId, const igIndex*& ptIds) override {
		ptIds = pointToOneRingPoints[pointId];
		return pointToOneRingPoints[pointId][MaxValence];
	}
	int GetPointToNeighborEdges(const int pointId, const igIndex*& edgeIds) override {
		edgeIds = pointToNeighborEdges[pointId];
		return pointToNeighborEdges[pointId][MaxValence];
	}
	int GetPointToNeighborFaces(const int pointId, const igIndex*& faceIds) override {
		faceIds = pointToNeighborFaces[pointId];
		return pointToNeighborFaces[pointId][MaxValence];
	}
	int GetEdgeToNeighborFaces(const int edgeId, const igIndex*& ptIds) override {
		ptIds = edgeToNeighborFaces[edgeId];
		return 2;
	}
	int GetFaceToNeighborFaces(const int faceId, const igIndex*& faceIds) override {
		faceIds = faceToNeighborFaces[faceId];
		return faceToNeighborFaces[faceId][MaxFaceSize];
	}

	static int EdgePointIds(const int edgeId, const igIndex*& ptIds) {
		ptIds = edges[edgeId];
		return 3;
	}
	static int FacePointIds(const int faceId, const igIndex*& ptIds) {
		ptIds = faces[faceId];
		return faces[faceId][MaxFaceSize];
	}
	static int FaceEdgeIds(const int faceId, const igIndex*& edgeIds) {
		edgeIds = faceEdges[faceId];
		return faceEdges[faceId][MaxFaceSize];
	}

	static int PointToOneRingPoints(const int pointId, const igIndex*& ptIds) {
		ptIds = pointToOneRingPoints[pointId];
		return pointToOneRingPoints[pointId][MaxValence];
	}
	static int PointToNeighborEdges(const int pointId, const igIndex*& edgeIds) {
		edgeIds = pointToNeighborEdges[pointId];
		return pointToNeighborEdges[pointId][MaxValence];
	}
	static int PointToNeighborFaces(const int pointId, const igIndex*& faceIds) {
		faceIds = pointToNeighborFaces[pointId];
		return pointToNeighborFaces[pointId][MaxValence];
	}
	static int EdgeToNeighborFaces(const int edgeId, const igIndex*& ptIds) {
		ptIds = edgeToNeighborFaces[edgeId];
		return 2;
	}
	static int FaceToNeighborFaces(const int faceId, const igIndex*& faceIds) {
		faceIds = faceToNeighborFaces[faceId];
		return faceToNeighborFaces[faceId][MaxFaceSize];
	}

private:
	QuadraticTetra()
	{
		this->Points->SetNumberOfPoints(10);
		this->PointIds->SetNumberOfIds(10);
		for (int i = 0; i < 10; i++)
		{
			this->Points->SetPoint(i, 0.0, 0.0, 0.0);
			this->PointIds->SetId(i, 0);
		}

		m_Line = QuadraticLine::New();
		m_Triangle = QuadraticTriangle::New();
	}
	~QuadraticTetra() override = default;

	QuadraticLine::Pointer m_Line;
	QuadraticTriangle::Pointer m_Triangle;
};


IGAME_NAMESPACE_END
#endif