#ifndef iGameTetra_h
#define iGameTetra_h

#include "iGameVolume.h"

IGAME_NAMESPACE_BEGIN
class Tetra : public Volume {
public:
	I_OBJECT(Tetra);
	static Pointer New() { return new Tetra; }
	
	int GetCellDimension() { return 3; }

	/**
	 * ��Ԫ�Ķ��������
	 */
	static constexpr int NumberOfPoints = 4;

	/**
	 * ��Ԫ�ı߸�����
	 */
	static constexpr int NumberOfEdges = 6;

	/**
	 * ��Ԫ���������
	 */
	static constexpr int NumberOfFaces = 4;

	/**
	 * ��Ԫ�����Ķ��������
	 */
	static constexpr int MaximumFaceSize = 3;

	/**
	 * ��Ԫ�������Ķȡ�
	 */
	static constexpr int MaximumValence = 3;

	/***************** ����Maximum + 1���������һλ���ֱ�ʾ���� *****************/

	// �ߵĶ������
	static constexpr int edges[NumberOfEdges][2] = {
	  { 0, 1 }, // 0
	  { 1, 2 }, // 1
	  { 2, 0 }, // 2
	  { 0, 3 }, // 3
	  { 1, 3 }, // 4
	  { 2, 3 }, // 5
	};

	// ��Ķ������
	static constexpr int faces[NumberOfFaces][MaximumFaceSize + 1] = {
	  { 0, 1, 3, 3 }, // 0
	  { 1, 2, 3, 3 }, // 1
	  { 2, 0, 3, 3 }, // 2
	  { 0, 2, 1, 3 }, // 3
	}; 

	// �ߵ��ڽ������
	static constexpr int edgeToNeighborFaces[NumberOfEdges][2] = {
	  { 0, 3 }, // 0
	  { 1, 3 }, // 1
	  { 2, 3 }, // 2
	  { 0, 2 }, // 3
	  { 0, 1 }, // 4
	  { 1, 2 }, // 5
	};

	// ����ڽ������
	static constexpr int faceToNeighborFaces[NumberOfFaces][MaximumFaceSize + 1] = {
	  { 3, 1, 2, 3 }, // 0
	  { 3, 2, 0, 3 }, // 1
	  { 3, 0, 1, 3 }, // 2
	  { 2, 1, 0, 3 }, // 3
	};

	// ������ڽӱ����
	static constexpr int pointToNeighborEdges[NumberOfPoints][MaximumValence + 1] = {
	  { 0, 3, 2, 3 }, // 0
	  { 0, 1, 4, 3 }, // 1
	  { 1, 2, 5, 3 }, // 2
	  { 3, 4, 5, 3 }, // 3
	};

	// ������ڽ������
	static constexpr int pointToNeighborFaces[NumberOfPoints][MaximumValence + 1] = {
	  { 0, 2, 3, 3 }, // 0
	  { 3, 1, 0, 3 }, // 1
	  { 3, 2, 1, 3 }, // 2
	  { 0, 1, 2, 3 }, // 3
	};

	// �����һ���򶥵����
	static constexpr int pointToOneRingPoints[NumberOfPoints][MaximumValence + 1] = {
	  { 1, 3, 2, 3 }, // 0
	  { 0, 2, 3, 3 }, // 1
	  { 1, 0, 3, 3 }, // 2
	  { 0, 1, 2, 3 }, // 3
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

	int GetCellType() override { return IG_TETRA; }
	int GetCellSize() override { return 4; }
	int GetNumberOfEdges() override { return 6; }
	int GetNumberOfFaces() override { return 4; }
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
		for (int i = 0; i < 3; ++i)
		{
			this->Triangle->PointIds->SetId(i, this->PointIds->GetId(verts[i]));
			this->Triangle->Points->SetPoint(i, this->Points->GetPoint(verts[i]));
		}

		return this->Triangle.get();
	}

private:
	Tetra()
	{
		this->Points->SetNumberOfPoints(4);
		this->PointIds->SetNumberOfIds(4);
		for (int i = 0; i < 4; i++)
		{
			this->Points->SetPoint(i, 0.0, 0.0, 0.0);
			this->PointIds->SetId(i, 0);
		}

		this->Line = Line::New();
		this->Triangle = Triangle::New();
	}
	~Tetra() override = default;

	Line::Pointer Line;
	Triangle::Pointer Triangle;
};


IGAME_NAMESPACE_END
#endif