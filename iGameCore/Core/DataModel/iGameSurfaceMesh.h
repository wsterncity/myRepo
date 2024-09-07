#ifndef iGameSurfaceMesh_h
#define iGameSurfaceMesh_h

#include "iGameCellArray.h"
#include "iGameCellLinks.h"
#include "iGameEdgeTable.h"
#include "iGamePointSet.h"

#include "iGameLine.h"
#include "iGamePolygon.h"
#include "iGameQuad.h"
#include "iGameTriangle.h"

#include "iGameMeshlet.h"

IGAME_NAMESPACE_BEGIN
class SurfaceMesh : public PointSet {
public:
  I_OBJECT(SurfaceMesh);
  static Pointer New() { return new SurfaceMesh; }

  // Get the number of all edges
  IGsize GetNumberOfEdges() const noexcept;
  // Get the number of all faces
  IGsize GetNumberOfFaces() const noexcept;

  // Get edge array
  CellArray *GetEdges();
  // Get face array
  CellArray *GetFaces();

  // Set face array
  void SetFaces(CellArray::Pointer faces);

  // Get edge cell by index edgeId
  Line *GetEdge(const IGsize edgeId);
  // Get face cell by index faceId
  Face *GetFace(const IGsize faceId);

  // Get edge's point index. Return PointIds size
  int GetEdgePointIds(const IGsize edgeId, igIndex *ptIds);
  // Get face's point index. Return PointIds size
  int GetFacePointIds(const IGsize faceId, igIndex *ptIds);
  // Get face's edge index. Return EdgeIds size
  int GetFaceEdgeIds(const IGsize faceId, igIndex *edgeIds);

  // All edges are constructed according to the point index of the face
  void BuildEdges();
  // Construct the adjacent edges of the points
  void BuildEdgeLinks();
  // Construct the adjacent faces of the points
  void BuildFaceLinks();
  // Construct the adjacent faces of the edges
  void BuildFaceEdgeLinks();

  enum Type {
      P2P = 0,
      P2E,
      P2F,
      P2V,
      E2F,
      E2RF,
      E2V,
      F2F,
      F2RF,
      F2V,
  };
  int GetNumberOfLinks(const IGsize id, Type type);
  // Get all points within one ring of a point. Return the size of indices.
  int GetPointToOneRingPoints(const IGsize ptId, igIndex *ptIds);
  // Get all neighboring edges of a point. Return the size of indices.
  int GetPointToNeighborEdges(const IGsize ptId, igIndex *edgeIds);
  // Get all neighboring faces of a point. Return the size of indices.
  int GetPointToNeighborFaces(const IGsize ptId, igIndex *faceIds);
  // Get all neighboring faces of a edge (shared edge). Return the size of
  // indices.
  int GetEdgeToNeighborFaces(const IGsize edgeId, igIndex *faceIds);
  // Get all faces within one ring of a edge (shared point). Return the size of
  // indices.
  int GetEdgeToOneRingFaces(const IGsize edgeId, igIndex *faceIds);
  // Get all neighboring faces of a face (shared edge). Return the size of
  // indices.
  int GetFaceToNeighborFaces(const IGsize faceId, igIndex *faceIds);
  // Get all faces within one ring of a face (shared point). Return the size of
  // indices.
  int GetFaceToOneRingFaces(const IGsize faceId, igIndex *faceIds);

  // Get edge index according to two point index. If don't, return index -1
  igIndex GetEdgeIdFormPointIds(const IGsize ptId1, const IGsize ptId2);
  // Get face index according to sequence. If don't, return index -1
  igIndex GetFaceIdFormPointIds(igIndex *ids, int size);

  // Request data edit state, only in this state,
  // can perform the adding and delete operation.
  // Adding operations also can be done via GetFaces().
  void RequestEditStatus() override;

  // Garbage collection to free memory that has been removed.
  // This function must be called if the topology changes.
  void GarbageCollection() override;

  // Whether edge is deleted or not
  bool IsEdgeDeleted(const IGsize edgeId);
  // Whether face is deleted or not
  bool IsFaceDeleted(const IGsize faceId);

  // Add element, necessarily called after RequestEditStatus()
  IGsize AddPoint(const Point &p) override;
  virtual IGsize AddEdge(const IGsize ptId1, const IGsize ptId2);
  virtual IGsize AddFace(igIndex *ptIds, int size);

  // Delete element, necessarily called after RequestEditStatus()
  void DeletePoint(const IGsize ptId) override;
  virtual void DeleteEdge(const IGsize edgeId);
  virtual void DeleteFace(const IGsize faceId);

  void ReplacePointReference(const IGsize fromPtId, const IGsize toPtId);
  virtual bool IsBoundryFace(igIndex);
  virtual bool IsBoundryEdge(igIndex);
  virtual bool IsBoundryPoint(igIndex);
  virtual bool IsCornerPoint(igIndex);

  bool IsOnBoundaryPoint(igIndex ptId);
  bool IsOnBoundaryEdge(igIndex edgeId);
  bool IsOnBoundaryFace(igIndex faceId);

protected:
  SurfaceMesh();
  ~SurfaceMesh() override = default;

  void RequestEdgeStatus();
  void RequestFaceStatus();

  DeleteMarker::Pointer m_EdgeDeleteMarker{};
  DeleteMarker::Pointer m_FaceDeleteMarker{};

  CellArray::Pointer m_Edges{};     // The point set of edges
  CellLinks::Pointer m_EdgeLinks{}; // The adjacent edges of points

  CellArray::Pointer m_Faces{};     // The point set of faces
  CellLinks::Pointer m_FaceLinks{}; // The adjacent faces of points

  CellArray::Pointer m_FaceEdges{};     // The edge set of faces
  CellLinks::Pointer m_FaceEdgeLinks{}; // The adjacent faces of edges

private:
  // Used for the returned cell object, which is Thread-Unsafe
  Line::Pointer m_Edge{};
  Triangle::Pointer m_Triangle{};
  Quad::Pointer m_Quad{};
  Polygon::Pointer m_Polygon{};

public:
  void Draw(Scene *) override;
  void DrawPhase1(Scene *) override;
  void DrawPhase2(Scene *) override;
  void TestOcclusionResults(Scene *) override;
  void ConvertToDrawableData() override;
  bool IsDrawable() override { return true; }
  void ViewCloudPicture(Scene* scene, int index, int demension = -1) override;
  void SetAttributeWithPointData(ArrayObject::Pointer attr,
                                 igIndex i = -1, const std::pair<float, float>& range = {0.f, 0.f}) override;
  void SetAttributeWithCellData(ArrayObject::Pointer attr, igIndex i = -1);

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

#ifdef IGAME_OPENGL_VERSION_460
  Meshlet::Pointer m_Meshlets{Meshlet::New()};
#endif

  bool m_Flag{false};
  bool m_UseColor{false};
  bool m_ColorWithCell{false};
  int m_PointSize{1};
  int m_LineWidth{1};

  ArrayObject::Pointer m_ViewAttribute{};
  int m_ViewDemension{-1};
};
IGAME_NAMESPACE_END
#endif