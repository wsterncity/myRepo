#ifndef iGamePyramid_h
#define iGamePyramid_h

#include "iGameVolume.h"

IGAME_NAMESPACE_BEGIN
class Pyramid : public Volume {
public:
	I_OBJECT(Pyramid);
	static Pointer New() { return new Pyramid; }

	int GetCellDimension() { return 3; }

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
	static constexpr int MaximumFaceSize = 4;

	/**
	 * ��Ԫ�������Ķȡ�
	 */
	static constexpr int MaximumValence = 4;

	/***************** ����Maximum + 1���������һλ���ֱ�ʾ���� *****************/

	// �ߵĶ������
	static constexpr int edges[NumberOfEdges][2] = {
	  { 0, 1 }, // 0
	  { 1, 2 }, // 1
	  { 2, 3 }, // 2
	  { 3, 0 }, // 3
	  { 0, 4 }, // 4
	  { 1, 4 }, // 5
	  { 2, 4 }, // 6
	  { 3, 4 }, // 7
	};

	// ��Ķ������
	static constexpr int faces[NumberOfFaces][MaximumFaceSize + 1] = {
	  { 0, 3, 2,  1, 4 }, // 0
	  { 0, 1, 4, -1, 3 }, // 1
	  { 1, 2, 4, -1, 3 }, // 2
	  { 2, 3, 4, -1, 3 }, // 3
	  { 3, 0, 4, -1, 3 }, // 4
	};

	// �ߵ��ڽ������
	static constexpr int edgeToNeighborFaces[NumberOfEdges][2] = {
	  { 0, 1 }, // 0
	  { 0, 2 }, // 1
	  { 0, 3 }, // 2
	  { 0, 4 }, // 3
	  { 1, 4 }, // 4
	  { 1, 2 }, // 5
	  { 2, 3 }, // 6
	  { 3, 4 }, // 7
	};

	// ����ڽ������
	static constexpr int faceToNeighborFaces[NumberOfFaces][MaximumFaceSize + 1] = {
	  { 4, 3, 2, 1,  4 }, // 0
	  { 0, 2, 4, -1, 3 }, // 1
	  { 0, 3, 1, -1, 3 }, // 2
	  { 0, 4, 2, -1, 3 }, // 3
	  { 0, 1, 3, -1, 3 }, // 4
	};

	// ������ڽӱ����
	static constexpr int pointToNeighborEdges[NumberOfPoints][MaximumValence + 1] = {
	  { 0, 4, 3, -1, 3 }, // 0
	  { 0, 1, 5, -1, 3 }, // 1
	  { 1, 2, 6, -1, 3 }, // 2
	  { 2, 3, 7, -1, 3 }, // 3
	  { 4, 5, 6, 7 , 4 }, // 4
	};

	// ������ڽ������
	static constexpr int pointToNeighborFaces[NumberOfPoints][MaximumValence + 1] = {
	  { 1, 4, 0, -1, 3 }, // 0
	  { 0, 2, 1, -1, 3 }, // 1
	  { 0, 3, 2, -1, 3 }, // 2
	  { 0, 4, 3, -1, 3 }, // 3
	  { 1, 2, 3, 4 , 4 }, // 4
	};

	// �����һ���򶥵����
	static constexpr int pointToOneRingPoints[NumberOfPoints][MaximumValence + 1] = {
	  { 1, 4, 3, -1, 3 }, // 0
	  { 0, 2, 4, -1, 3 }, // 1
	  { 1, 3, 4, -1, 3 }, // 2
	  { 2, 0, 4, -1, 3 }, // 3
	  { 0, 1, 2, 3 , 4 }, // 4
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

	int GetCellType() override { return IG_PYRAMID; }
	int GetCellSize() override { return 5; }
	int GetNumberOfEdges() override { return 8; }
	int GetNumberOfFaces() override { return 5; }
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
		Cell* face;
		if (faces[faceId][MaximumFaceSize] == 3)
		{
			face = this->Triangle.get();
		}
		else 
		{
			face = this->Quad.get();
		}
		for (int i = 0; i < faces[faceId][MaximumFaceSize]; ++i)
		{
			face->PointIds->SetId(i, this->PointIds->GetId(verts[i]));
			face->Points->SetPoint(i, this->Points->GetPoint(verts[i]));
		}

		return face;
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

		this->Line = Line::New();
		this->Triangle = Triangle::New();
		this->Quad = Quad::New();
	}
	~Pyramid() override = default;

	Line::Pointer Line;
	Triangle::Pointer Triangle;
	Quad::Pointer Quad;
};


IGAME_NAMESPACE_END
#endif