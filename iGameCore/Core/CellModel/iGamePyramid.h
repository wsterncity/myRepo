#ifndef iGamePyramid_h
#define iGamePyramid_h

#include "iGameVolume.h"

IGAME_NAMESPACE_BEGIN
class Pyramid : public Volume {
public:
	I_OBJECT(Pyramid);
	static Pointer New() { return new Pyramid; }

	IGenum GetCellType() const noexcept override { return IG_PYRAMID; }
    int GetCellSize() const noexcept override { return 5; }
    int GetNumberOfEdges() override { return 8; }
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
	 * ��Ԫ�Ķ��������
	 */
	static constexpr int NumberOfPoints = 5;

	/**
	 * ��Ԫ�ı߸�����
	 */
	static constexpr int NumberOfEdges = 8;

	/**
	 * ��Ԫ���������
	 */
	static constexpr int NumberOfFaces = 5;

	/**
	 * ��Ԫ�����Ķ��������
	 */
	static constexpr int MaxFaceSize = 4;

	/**
	 * ��Ԫ�������Ķȡ�
	 */
	static constexpr int MaxValence = 4;

	/***************** ����Max + 1���������һλ���ֱ�ʾ���� *****************/

	// �ߵĶ������
	static constexpr int edges[NumberOfEdges][2] = {
	  { 0, 1 },
	  { 1, 2 },
	  { 2, 3 },
	  { 3, 0 },
	  { 0, 4 },
	  { 1, 4 },
	  { 2, 4 },
	  { 3, 4 },
	};

	// ��Ķ������
	static constexpr int faces[NumberOfFaces][MaxFaceSize + 1] = {
	  { 0, 3, 2,  1, 4 },
	  { 0, 1, 4, -1, 3 },
	  { 1, 2, 4, -1, 3 },
	  { 2, 3, 4, -1, 3 },
	  { 3, 0, 4, -1, 3 },
	};

	// ��ı����
	static constexpr int faceEdges[NumberOfFaces][MaxFaceSize + 1] = {
	  { 0, 4, 7, 3, 4 },//��û��
	  { 1, 2, 6, 5, 4 },
	  { 0, 1, 5, 4, 4 },
	  { 3, 7, 6, 2, 4 },
	  { 0, 3, 2, 1, 4 },
	};

	// �ߵ��ڽ������
	static constexpr int edgeToNeighborFaces[NumberOfEdges][2] = {
	  { 0, 1 },
	  { 0, 2 },
	  { 0, 3 },
	  { 0, 4 },
	  { 1, 4 },
	  { 1, 2 },
	  { 2, 3 },
	  { 3, 4 },
	};

	// ����ڽ������
	static constexpr int faceToNeighborFaces[NumberOfFaces][MaxFaceSize + 1] = {
	  { 4, 3, 2, 1,  4 },
	  { 0, 2, 4, -1, 3 },
	  { 0, 3, 1, -1, 3 },
	  { 0, 4, 2, -1, 3 },
	  { 0, 1, 3, -1, 3 },
	};

	// ������ڽӱ����
	static constexpr int pointToNeighborEdges[NumberOfPoints][MaxValence + 1] = {
	  { 0, 4, 3, -1, 3 },
	  { 0, 1, 5, -1, 3 },
	  { 1, 2, 6, -1, 3 },
	  { 2, 3, 7, -1, 3 },
	  { 4, 5, 6, 7 , 4 },
	};

	// ������ڽ������
	static constexpr int pointToNeighborFaces[NumberOfPoints][MaxValence + 1] = {
	  { 1, 4, 0, -1, 3 },
	  { 0, 2, 1, -1, 3 },
	  { 0, 3, 2, -1, 3 },
	  { 0, 4, 3, -1, 3 },
	  { 1, 2, 3, 4 , 4 },
	};

	// �����һ���򶥵����
	static constexpr int pointToOneRingPoints[NumberOfPoints][MaxValence + 1] = {
	  { 1, 4, 3, -1, 3 },
	  { 0, 2, 4, -1, 3 },
	  { 1, 3, 4, -1, 3 },
	  { 2, 0, 4, -1, 3 },
	  { 0, 1, 2, 3 , 4 },
	};

	int GetEdgePointIds(const int edgeId, const igIndex*& pts) override {
		pts = edges[edgeId];
        return 2;
	}
    int GetFacePointIds(const int faceId, const igIndex*& pts) override {
		pts = faces[faceId];
		return faces[faceId][MaxFaceSize];
	}
	int GetFaceEdgeIds(const int faceId, const igIndex*& edgeIds) override {
		edgeIds = faceEdges[faceId];
		return faceEdges[faceId][MaxFaceSize];
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
	Pyramid()
	{
		this->Points->SetNumberOfPoints(5);
		this->PointIds->SetNumberOfIds(5);
		for (int i = 0; i < 5; i++)
		{
			this->Points->SetPoint(i, 0.0, 0.0, 0.0);
			this->PointIds->SetId(i, 0);
		}

		m_Line = Line::New();
		m_Triangle = Triangle::New();
		m_Quad = Quad::New();
	}
	~Pyramid() override = default;

	Line::Pointer m_Line;
    Triangle::Pointer m_Triangle;
    Quad::Pointer m_Quad;
};


IGAME_NAMESPACE_END
#endif