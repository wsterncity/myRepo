#ifndef iGameSurfaceMesh_h
#define iGameSurfaceMesh_h

#include "iGamePointSet.h"
#include "iGameCellArray.h"
#include "iGameEdgeTable.h"
#include "iGameCellLinks.h"

#include "iGameLine.h"
#include "iGameTriangle.h"
#include "iGameQuad.h"
#include "iGamePolygon.h"

#include "iGameMeshlet.h"

IGAME_NAMESPACE_BEGIN
class SurfaceMesh : public PointSet {
public:
	I_OBJECT(SurfaceMesh);
	static Pointer New() { return new SurfaceMesh; }

	igIndex GetNumberOfEdges();
	igIndex GetNumberOfFaces();
	
	CellArray* GetEdges();
	CellArray* GetFaces();

	void SetFaces(CellArray::Pointer faces);

	Line* GetEdge(const IGsize edgeId);
	Face* GetFace(const IGsize faceId);

	int GetEdgePointIds(const IGsize edgeId, igIndex* ptIds);
	int GetFacePointIds(const IGsize faceId, igIndex* ptIds);
	int GetFaceEdgeIds(const IGsize faceId, igIndex* edgeIds);

	void BuildEdges();
	void BuildEdgeLinks();
	void BuildFaceLinks();
	void BuildFaceEdgeLinks();

	int GetPointToOneRingPoints(const IGsize ptId, igIndex* ptIds);
	int GetPointToNeighborEdges(const IGsize ptId, igIndex* edgeIds);
	int GetPointToNeighborFaces(const IGsize ptId, igIndex* faceIds);
	int GetEdgeToNeighborFaces(const IGsize edgeId, igIndex* faceIds);
	int GetEdgeToOneRingFaces(const IGsize edgeId, igIndex* faceIds);
	int GetFaceToNeighborFaces(const IGsize faceId, igIndex* faceIds);
	int GetFaceToOneRingFaces(const IGsize faceId, igIndex* faceIds);

	void RequestEditStatus() override
	{
		if (InEditStatus())
		{
			return;
		}
		RequestPointStatus();
		RequestEdgeStatus();
		RequestFaceStatus();
		MakeEditStatusOn();
	}

	igIndex GetEdgeIdFormPointIds(const IGsize ptId1, const IGsize ptId2)
	{
		igIndex edgeIds[64]{}, e[2]{};
		int size = GetPointToNeighborEdges(ptId1, edgeIds);
		for (int i = 0; i < size; i++)
		{
			GetEdgePointIds(edgeIds[i], e);
			if (ptId1 + ptId2 == e[0] + e[1]) {
				return edgeIds[i];
			}
		}
		return (-1);
	}
	igIndex GetFaceIdFormPointIds(igIndex* ids, int size)
	{
		IGsize sum = 0;
		for (int i = 0; i < size; i++) {
			if (ids[i] >= this->GetNumberOfPoints()) {
				return (-1);
			}
			sum += ids[i];
		}

		igIndex faceIds[64]{}, ptIds[32]{};
		int size1 = GetPointToNeighborFaces(ids[0], faceIds);
		for (int i = 0; i < size1; i++)
		{
			if (size != GetFacePointIds(faceIds[i], ptIds)) continue;
			IGsize index_sum = 0;
			for (int j = 0; j < size; j++)
			{
				index_sum += ptIds[j];
			}
			if (sum == index_sum)
			{
				int count = 0;
				for (int j = 0; j < size; j++) {
					for (int k = 0; k < size; k++) {
						if (ids[j] == ptIds[k]) {
							count++;
							break;
						}
					}
				}
				if (count == size) return faceIds[i];
			}
		}
		return (-1);
	}

	IGsize AddPoint(const Point& p) override
	{
		if (!InEditStatus())
		{
			RequestEditStatus();
		}
		IGsize ptId = m_Points->AddPoint(p);

		m_EdgeLinks->AddLink();
		m_FaceLinks->AddLink();

		m_PointDeleteMarker->AddTag();
		Modified();
		return ptId;
	}
	IGsize AddEdge(const IGsize ptId1, const IGsize ptId2)
	{
		igIndex edgeId = GetEdgeIdFormPointIds(ptId1, ptId2);
		if (edgeId == -1) {
			edgeId = GetNumberOfEdges();
			m_Edges->AddCellId2(ptId1, ptId2);
			m_EdgeLinks->AddReference(ptId1, edgeId);
			m_EdgeLinks->AddReference(ptId2, edgeId);
			m_FaceEdgeLinks->AddLink();
			m_EdgeDeleteMarker->AddTag();
			Modified();
		}
		return edgeId;
	}
	IGsize AddFace(igIndex* ptIds, int size)
	{
		igIndex edgeIds[64]{};
		for (int i = 0; i < size; i++) {
			edgeIds[i] = AddEdge(ptIds[i], ptIds[(i + 1) % size]);
		}
		igIndex faceId = GetFaceIdFormPointIds(ptIds, size);
		if (faceId == -1) {
			faceId = GetNumberOfFaces();
			m_Faces->AddCellIds(ptIds, size);
			m_FaceEdges->AddCellIds(edgeIds, size);
			for (int i = 0; i < size; i++) {
				m_FaceLinks->AddReference(ptIds[i], faceId);
				m_FaceEdgeLinks->AddReference(edgeIds[i], faceId);
			}

			m_FaceDeleteMarker->AddTag();
			Modified();
		}

		return faceId;
	}

	void DeletePoint(const IGsize ptId) override
	{
		if (!InEditStatus())
		{
			RequestEditStatus();
		}
		if (IsPointDeleted(ptId))
		{
			return;
		}
		igIndex edgeIds[64]{};
		int size = GetPointToNeighborEdges(ptId, edgeIds);
		for (int i = 0; i < size; i++) {
			DeleteEdge(edgeIds[i]);
		}
		m_EdgeLinks->DeleteLink(ptId);
		m_FaceLinks->DeleteLink(ptId);
		m_PointDeleteMarker->MarkDeleted(ptId);
	}
	void DeleteEdge(const IGsize edgeId) {
		if (!InEditStatus())
		{
			RequestEditStatus();
		}
		if (IsEdgeDeleted(edgeId))
		{
			return;
		}
		igIndex faceIds[64]{}, e[2]{};
		int size = GetEdgeToNeighborFaces(edgeId, faceIds);
		GetEdgePointIds(edgeId, e);
		for (int i = 0; i < 2; i++) {
			m_EdgeLinks->RemoveReference(edgeId, e[i]);
		}
		for (int i = 0; i < size; i++) {
			DeleteFace(faceIds[i]);
		}
		m_FaceEdgeLinks->DeleteLink(edgeId);
		m_EdgeDeleteMarker->MarkDeleted(edgeId);
	}
	void DeleteFace(const IGsize faceId) {
		if (!InEditStatus())
		{
			RequestEditStatus();
		}
		if (IsFaceDeleted(faceId))
		{
			return;
		}
		igIndex ptIds[64]{}, edgeIds[64]{};
		int size = GetFacePointIds(faceId, ptIds);
		GetFaceEdgeIds(faceId, edgeIds);
		for (int i = 0; i < size; i++) {
			m_FaceLinks->RemoveReference(faceId, ptIds[i]);
			m_FaceEdgeLinks->RemoveReference(faceId, edgeIds[i]);
		}
		m_FaceDeleteMarker->MarkDeleted(faceId);
	}

	void GarbageCollection() override
	{
		IGsize i, mappedPtId = 0, mappedEdgeId = 0;
		igIndex ptIds[32]{}, edgeIds[32]{}, e[2]{};
		CellArray::Pointer newEdges = CellArray::New();
		CellArray::Pointer newFaces = CellArray::New();
		CellArray::Pointer newFaceEdges = CellArray::New();

		IGsize npts = GetNumberOfPoints();
		IGsize nedges = GetNumberOfEdges();
		IGsize nfaces = GetNumberOfFaces();

		std::vector<igIndex> ptMap(npts);
		std::vector<igIndex> edgeMap(nedges);

		for (i = 0; i < npts; i++) {
			if (IsPointDeleted(i)) continue;
			if (i != mappedPtId) {
				m_Points->SetPoint(mappedPtId, m_Points->GetPoint(i));
			}
			ptMap[i] = mappedPtId;
			mappedPtId++;
		}
		m_Points->Resize(mappedPtId);

		for (i = 0; i < nedges; i++) {
			if (IsEdgeDeleted(i)) continue;
			m_Edges->GetCellIds(i, e);
			for (int j = 0; j < 2; j++) {
				e[j] = ptMap[e[j]];
			}
			newEdges->AddCellIds(e, 2);
			edgeMap[i] = mappedEdgeId;
			mappedEdgeId++;
		}

		for (i = 0; i < nfaces; i++) {
			if (IsFaceDeleted(i)) continue;
			m_FaceEdges->GetCellIds(i, edgeIds);
			int size = m_Faces->GetCellIds(i, ptIds);
			for (int j = 0; j < size; j++) {
				ptIds[j] = ptMap[ptIds[j]];
				edgeIds[j] = edgeMap[edgeIds[j]];
			}
			newFaces->AddCellIds(ptIds, size);
			newFaceEdges->AddCellIds(edgeIds, size);
		}

		m_Edges = newEdges;
		m_Faces = newFaces;
		m_FaceEdges = newFaceEdges;
		m_EdgeLinks = nullptr;
		m_FaceLinks = nullptr;
		m_FaceEdgeLinks = nullptr;
		m_PointDeleteMarker = nullptr;
		m_EdgeDeleteMarker = nullptr;
		m_FaceDeleteMarker = nullptr;
		Modified();
		MakeEditStatusOff();
	}

	bool IsEdgeDeleted(const IGsize edgeId) {
		return m_EdgeDeleteMarker->IsDeleted(edgeId);
	}
	bool IsFaceDeleted(const IGsize faceId) {
		return m_FaceDeleteMarker->IsDeleted(faceId);
	}

	void ReplacePointReference(const IGsize fromPtId, const IGsize toPtId)
	{
		assert(fromPtId < GetNumberOfPoints() && "ptId too large");
		assert(toPtId < GetNumberOfPoints() && "ptId too large");
		if (fromPtId == toPtId)
		{
			return;
		}
		if (!InEditStatus())
		{
			RequestEditStatus();
		}
		igIndex edgeIds[64]{}, faceIds[64]{};
		int size1 = GetPointToNeighborEdges(fromPtId, edgeIds);
		int size2 = GetPointToNeighborFaces(fromPtId, faceIds);
		for (int i = 0; i < size1; i++) {
			m_Edges->ReplaceCellReference(edgeIds[i], fromPtId, toPtId);
		}
		for (int i = 0; i < size2; i++) {
			m_Faces->ReplaceCellReference(faceIds[i], fromPtId, toPtId);
		}

		auto& link1 = m_EdgeLinks->GetLink(fromPtId);
		m_EdgeLinks->SetLink(toPtId, link1.pointer, link1.size);

		auto& link2 = m_FaceLinks->GetLink(fromPtId);
		m_FaceLinks->SetLink(toPtId, link2.pointer, link2.size);
	}

protected:
	SurfaceMesh() 
	{
		m_ViewStyle = IG_SURFACE;
	};
	~SurfaceMesh() override {};

	void RequestEdgeStatus()
	{
		if (m_Edges == nullptr ||
			(m_Edges->GetMTime() < m_Faces->GetMTime()))
		{
			BuildEdges();

		}
		if (m_EdgeLinks == nullptr ||
			(m_EdgeLinks->GetMTime() < m_Edges->GetMTime()))
		{
			BuildEdgeLinks();
		}

		if (m_EdgeDeleteMarker == nullptr)
		{
			m_EdgeDeleteMarker = DeleteMarker::New();
		}
		m_EdgeDeleteMarker->Initialize(this->GetNumberOfEdges());
	}

	void RequestFaceStatus()
	{
		if (m_FaceEdgeLinks == nullptr ||
			(m_FaceEdgeLinks->GetMTime() < m_FaceEdges->GetMTime()))
		{
			BuildFaceEdgeLinks();
		}
		if (m_FaceLinks == nullptr ||
			(m_FaceLinks->GetMTime() < m_Faces->GetMTime()))
		{
			BuildFaceLinks();
		}

		if (m_FaceDeleteMarker == nullptr)
		{
			m_FaceDeleteMarker = DeleteMarker::New();
		}
		m_FaceDeleteMarker->Initialize(this->GetNumberOfFaces());
	}

	DeleteMarker::Pointer m_EdgeDeleteMarker{};
	DeleteMarker::Pointer m_FaceDeleteMarker{};

	CellArray::Pointer m_Edges{};        // The point set of edges
	CellLinks::Pointer m_EdgeLinks{};    // The adjacent edges of points

	CellArray::Pointer m_Faces{};        // The point set of faces
	CellLinks::Pointer m_FaceLinks{};    // The adjacent faces of points

	CellArray::Pointer m_FaceEdges{};    // The edge set of faces
	CellLinks::Pointer m_FaceEdgeLinks{};// The adjacent faces of edges

private:
	Line::Pointer m_Edge{};            // Used for the returned 'Line' object, which is Thread-Unsafe
	Triangle::Pointer m_Triangle{};    // Used for the returned 'Triangle' object, which is Thread-Unsafe
	Quad::Pointer m_Quad{};            // Used for the returned 'Quad' object, which is Thread-Unsafe
	Polygon::Pointer m_Polygon{};      // Used for the returned 'Polygon' object, which is Thread-Unsafe
	
public:
	void Draw(Scene*) override;
	void ConvertToDrawableData() override;
	bool IsDrawable() override { return true; }
	void ViewCloudPicture(int index, int demension = -1) override
	{
		if (index == -1)
		{
			m_UseColor = false;
			m_ViewAttribute = nullptr;
			m_ViewDemension = -1;
			m_ColorWithCell = false;
			return;
		}
		m_AttributeIndex = index;
		auto& attr = this->GetPropertySet()->GetProperty(index);
		if (!attr.isDeleted)
		{
			if(attr.attachmentType == IG_POINT)
				this->SetAttributeWithPointData(attr.pointer, demension);
			else if (attr.attachmentType == IG_CELL)
				this->SetAttributeWithCellData(attr.pointer, demension);
		}
	}

	void SetAttributeWithPointData(ArrayObject::Pointer attr, igIndex i = -1) override
	{
		if (m_ViewAttribute != attr || m_ViewDemension != i)
		{
			m_ViewAttribute = attr;
			m_ViewDemension = i;
			m_UseColor = true;
			m_ColorWithCell = false;
			ScalarsToColors::Pointer mapper = ScalarsToColors::New();

			if (i == -1)
			{
				mapper->InitRange(attr);
			}
			else
			{
				mapper->InitRange(attr, i);
			}

			m_Colors = mapper->MapScalars(attr, i);
			if (m_Colors == nullptr)
			{
				return;
			}

			GLAllocateGLBuffer(m_ColorVBO,
				m_Colors->GetNumberOfValues() * sizeof(float),
				m_Colors->RawPointer());

			m_PointVAO.vertexBuffer(GL_VBO_IDX_1, m_ColorVBO, 0, 3 * sizeof(float));
			GLSetVertexAttrib(m_PointVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
				GL_FALSE, 0);

			m_LineVAO.vertexBuffer(GL_VBO_IDX_1, m_ColorVBO, 0, 3 * sizeof(float));
			GLSetVertexAttrib(m_LineVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
				GL_FALSE, 0);


			m_TriangleVAO.vertexBuffer(GL_VBO_IDX_1, m_ColorVBO, 0, 3 * sizeof(float));
			GLSetVertexAttrib(m_TriangleVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
				GL_FALSE, 0);
		}
	}

	void SetAttributeWithCellData(ArrayObject::Pointer attr, igIndex i = -1)
	{
		if (m_ViewAttribute != attr || m_ViewDemension != i)
		{
			m_ViewAttribute = attr;
			m_ViewDemension = i;
			m_UseColor = true;
			m_ColorWithCell = true;
			ScalarsToColors::Pointer mapper = ScalarsToColors::New();

			if (i == -1)
			{
				mapper->InitRange(attr);
			}
			else
			{
				mapper->InitRange(attr, i);
			}

			FloatArray::Pointer colors = mapper->MapScalars(attr, i);
			if (colors == nullptr)
			{
				return;
			}

			FloatArray::Pointer newPositions = FloatArray::New();
			FloatArray::Pointer newColors = FloatArray::New();
			newPositions->SetElementSize(3);
			newColors->SetElementSize(3);

			float color[3]{};
			for (int i = 0; i < this->GetNumberOfFaces(); i++)
			{
				Face* face = this->GetFace(i);
				for (int j = 2; j < face->GetCellSize(); j++) {
					auto& p0 = face->Points->GetPoint(0);
					newPositions->AddElement3(p0[0], p0[1], p0[2]);

					auto& p1 = face->Points->GetPoint(j - 1);
					newPositions->AddElement3(p1[0], p1[1], p1[2]);

					auto& p2 = face->Points->GetPoint(j);
					newPositions->AddElement3(p2[0], p2[1], p2[2]);

					colors->GetElement(i, color);
					newColors->AddElement3(color[0], color[1], color[2]);
					newColors->AddElement3(color[0], color[1], color[2]);
					newColors->AddElement3(color[0], color[1], color[2]);
				}
			}
			m_CellPositionSize = newPositions->GetNumberOfElements();

			GLAllocateGLBuffer(m_CellPositionVBO,
				newPositions->GetNumberOfValues() * sizeof(float),
				newPositions->RawPointer());
			GLAllocateGLBuffer(m_CellColorVBO,
				newColors->GetNumberOfValues() * sizeof(float),
				newColors->RawPointer());

			m_CellVAO.vertexBuffer(GL_VBO_IDX_0, m_CellPositionVBO, 0, 3 * sizeof(float));
			GLSetVertexAttrib(m_CellVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 3, GL_FLOAT,
				GL_FALSE, 0);
			m_CellVAO.vertexBuffer(GL_VBO_IDX_1, m_CellColorVBO, 0, 3 * sizeof(float));
			GLSetVertexAttrib(m_CellVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_1, 3, GL_FLOAT,
				GL_FALSE, 0);
		}
	}

private:
	GLVertexArray m_PointVAO, m_LineVAO, m_TriangleVAO;
	GLBuffer m_PositionVBO, m_ColorVBO, m_NormalVBO, m_TextureVBO;
	GLBuffer m_PointEBO, m_LineEBO, m_TriangleEBO;

	GLVertexArray m_CellVAO;
	GLBuffer m_CellPositionVBO, m_CellColorVBO;
	int m_CellPositionSize{};

	FloatArray::Pointer m_Positions{};
	FloatArray::Pointer m_Colors{};
	IdArray::Pointer m_PointIndices{};
	IdArray::Pointer m_LineIndices{};
	IdArray::Pointer m_TriangleIndices{};
    Meshlet::Pointer m_Meshlets = Meshlet::New();

	bool m_Flag{ false };
	bool m_UseColor{ false };
	bool m_ColorWithCell{ false };
	int m_PointSize{ 1 };
	int m_LineWidth{ 1 };

	ArrayObject::Pointer m_ViewAttribute{};
	int m_ViewDemension{-1};
};
IGAME_NAMESPACE_END
#endif