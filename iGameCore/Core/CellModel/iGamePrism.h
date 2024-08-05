#ifndef iGamePrism_h
#define iGamePrism_h

#include "iGameVolume.h"

IGAME_NAMESPACE_BEGIN
class Prism : public Volume {
public:
	I_OBJECT(Prism);
	static Pointer New() { return new Prism; }

	IGenum GetCellType() const noexcept override { return IG_PRISM; }
    int GetCellSize() const noexcept override { return 6; }
    int GetNumberOfEdges() override { return 9; }
    int GetNumberOfFaces() override { return 5; }

    Cell* GetEdge(const int edgeId) override {
        const int* verts = edges[edgeId];

        m_Line->PointIds->SetId(0, this->PointIds->GetId(verts[0]));
        m_Line->PointIds->SetId(1, this->PointIds->GetId(verts[1]));

        m_Line->Points->SetPoint(0, this->Points->GetPoint(verts[0]));
        m_Line->Points->SetPoint(1, this->Points->GetPoint(verts[1]));

        return m_Line.get();
    }
    Cell* GetFace(const int faceId) override {
        const int* verts = faces[faceId];
        Cell* face;
        if (faces[faceId][MaxFaceSize] == 3) {
            face = m_Triangle.get();
        } else {
            face = m_Quad.get();
        }
        for (int i = 0; i < faces[faceId][MaxFaceSize]; ++i) {
            face->PointIds->SetId(i, this->PointIds->GetId(verts[i]));
            face->Points->SetPoint(i, this->Points->GetPoint(verts[i]));
        }

        return face;
    }

	/**
	 * 单元的顶点个数。
	 */
	static constexpr int NumberOfPoints = 6;

	/**
	 * 单元的边个数。
	 */
	static constexpr int NumberOfEdges = 9;

	/**
	 * 单元的面个数。
	 */
	static constexpr int NumberOfFaces = 5;

	/**
	 * 单元面最大的顶点个数。
	 */
	static constexpr int MaxFaceSize = 4;

	/**
	 * 单元顶点最大的度。
	 */
	static constexpr int MaxValence = 3;

	/***************** 所有Maximum + 1的数组最后一位数字表示个数 *****************/

	// 边的顶点序号
	static constexpr int edges[NumberOfEdges][2] = {
	  { 0, 1 },
	  { 1, 2 },
	  { 2, 0 },
	  { 3, 4 },
	  { 4, 5 },
	  { 5, 3 },
	  { 0, 3 },
	  { 1, 4 },
	  { 2, 5 },
	};

	// 面的顶点序号
	static constexpr int faces[NumberOfFaces][MaxFaceSize + 1] = {
	  { 0, 1, 2, -1, 3 },
	  { 3, 5, 4, -1, 3 },
	  { 0, 3, 4, 1,  4 },
	  { 1, 4, 5, 2,  4 },
	  { 2, 5, 3, 0,  4 },
	};

	// 边的邻接面序号
	static constexpr int edgeToNeighborFaces[NumberOfEdges][2] = {
	  { 0, 2 },
	  { 0, 3 },
	  { 0, 3 },
	  { 1, 2 },
	  { 1, 3 },
	  { 1, 4 },
	  { 2, 4 },
	  { 2, 3 },
	  { 3, 4 },
	};

	// 面的邻接面序号
	static constexpr int faceToNeighborFaces[NumberOfFaces][MaxFaceSize + 1] = {
	  { 4, 3, 2, -1, 3 },
	  { 2, 3, 4, -1, 3 },
	  { 0, 3, 1, 4 , 4 },
	  { 0, 4, 1, 2 , 4 },
	  { 0, 2, 1, 3 , 4 },
	};

	// 顶点的邻接边序号
	static constexpr int pointToNeighborEdges[NumberOfPoints][MaxValence + 1] = {
	  { 0, 6, 2, 3 },
	  { 0, 1, 7, 3 },
	  { 1, 2, 8, 3 },
	  { 3, 5, 6, 3 },
	  { 3, 7, 4, 3 },
	  { 4, 8, 5, 3 },
	};

	// 顶点的邻接面序号
	static constexpr int pointToNeighborFaces[NumberOfPoints][MaxValence + 1] = {
	  { 2, 4, 0, 3 },
	  { 0, 3, 2, 3 },
	  { 0, 4, 3, 3 },
	  { 1, 4, 2, 3 },
	  { 2, 3, 1, 3 },
	  { 3, 4, 1, 3 },
	};

	// 顶点的一邻域顶点序号
	static constexpr int pointToOneRingPoints[NumberOfPoints][MaxValence + 1] = {
	  { 1, 3, 2, 3 },
	  { 0, 2, 4, 3 },
	  { 1, 0, 5, 3 },
	  { 4, 5, 0, 3 },
	  { 3, 1, 5, 3 },
	  { 4, 2, 3, 3 },
	};

	int GetEdgePointIds(const int edgeId, const igIndex*& pts) override {
		pts = edges[edgeId];
        return 2;
	}
    int GetFacePointIds(const int faceId, const igIndex*& pts) override {
		pts = faces[faceId];
		return faces[faceId][MaxFaceSize];
	}
    int GetEdgeToNeighborFaces(const int edgeId, const igIndex*& pts) override {
		pts = edgeToNeighborFaces[edgeId];
        return 2;
	}
	int GetFaceToNeighborFaces(const int faceId, const igIndex*& faceIds) override {
		faceIds = faceToNeighborFaces[faceId];
		return faceToNeighborFaces[faceId][MaxFaceSize];
	}
	int GetPointToNeighborEdges(const int pointId, const igIndex*& edgeIds) override {
		edgeIds = pointToNeighborEdges[pointId];
		return pointToNeighborEdges[pointId][MaxValence];
	}
	int GetPointToNeighborFaces(const int pointId, const igIndex*& faceIds) override {
		faceIds = pointToNeighborFaces[pointId];
		return pointToNeighborFaces[pointId][MaxValence];
	}
	int GetPointToOneRingPoints(const int pointId, const igIndex*& pts) override {
		pts = pointToOneRingPoints[pointId];
		return pointToOneRingPoints[pointId][MaxValence];
	}

private:
	Prism()
	{
		this->Points->SetNumberOfPoints(6);
		this->PointIds->SetNumberOfIds(6);
		for (int i = 0; i < 6; i++)
		{
			this->Points->SetPoint(i, 0.0, 0.0, 0.0);
			this->PointIds->SetId(i, 0);
		}

		m_Line = Line::New();
		m_Triangle = Triangle::New();
		m_Quad = Quad::New();
	}
	~Prism() override = default;

	Line::Pointer m_Line;
    Triangle::Pointer m_Triangle;
    Quad::Pointer m_Quad;
};


IGAME_NAMESPACE_END
#endif