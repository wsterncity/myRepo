#ifndef iGameVolumeMesh_2_h
#define iGameVolumeMesh_2_h

#include "iGameSurfaceMesh.h"

#include "iGameHexahedron.h"
#include "iGamePrism.h"
#include "iGamePyramid.h"
#include "iGameTetra.h"

IGAME_NAMESPACE_BEGIN
class VolumeMesh_2 : public SurfaceMesh {
public:
    I_OBJECT(VolumeMesh_2);
    static Pointer New() { return new VolumeMesh_2; }

    // Get the number of all volumes
    IGsize GetNumberOfVolumes() const noexcept;

    // Get/Set volume array
    CellArray* GetVolumes();
    void SetVolumes(CellArray::Pointer volumes);
    void SetVolumeFaces(CellArray::Pointer faces);

    // Get edge cell by index volumeId
    Volume* GetVolume(const IGsize volumeId);

    // Get volume's point index. Return PointIds size
    int GetVolumePointIds(const IGsize volumeId, igIndex* ptIds);
    // Get volume's edge index. Return EdgeIds size
    int GetVolumeEdgeIds(const IGsize volumeId, igIndex* edgeIds);
    // Get volume's face index. Return FaceIds size
    int GetVolumeFaceIds(const IGsize volumeId, igIndex* faceIds);

    // Construct all the faces and add the face index to VolumeFaces
    void BuildFaces();
    // Construct all the faces and edges. Add the face index to VolumeFaces.
    // Add the edge index to VolumeEdges,
    void BuildFacesAndEdges();
    // Construct the adjacent volumes of the points
    void BuildVolumeLinks();
    // Construct the adjacent volumes of the edges
    void BuildVolumeEdgeLinks();
    // Construct the adjacent volumes of the faces
    void BuildVolumeFaceLinks();

    // Get all neighboring volumes of a point. Return the size of indices.
    int GetPointToNeighborVolumes(const IGsize ptId, igIndex* volumeIds);
    // Get all neighboring volumes of a edge. Return the size of indices.
    int GetEdgeToNeighborVolumes(const IGsize edgeId, igIndex* volumeIds);
    // Get all neighboring volumes of a face. Return the size of indices.
    int GetFaceToNeighborVolumes(const IGsize faceId, igIndex* volumeIds);
    // Get all neighboring volumes of a volume (Shared point). Return the size of indices.
    int GetVolumeToNeighborVolumesWithPoint(const IGsize volumeId,
        igIndex* volumeIds);
    // Get all neighboring volumes of a volume (Shared edge). Return the size of indices.
    int GetVolumeToNeighborVolumesWithEdge(const IGsize volumeId,
        igIndex* volumeIds);
    // Get all neighboring volumes of a volume (Shared face). Return the size of indices.
    int GetVolumeToNeighborVolumesWithFace(const IGsize volumeId,
        igIndex* volumeIds);

    // Get volume index according to sequence. If don't, return index -1
    igIndex GetVolumeIdFormPointIds(igIndex* ids, int size);

    // Request data edit state, only in this state,
    // can perform the adding and delete operation.
    // Adding operations also can be done via GetVolumes().
    void RequestEditStatus() override;

    // Garbage collection to free memory that has been removed.
    // This function must be called if the topology changes.
    void GarbageCollection() override;

    // Whether volume is deleted or not
    bool IsVolumeDeleted(const IGsize volumeId);

    // Add element, necessarily called after RequestEditStatus()
    IGsize AddPoint(const Point& p) override;
    IGsize AddEdge(const IGsize ptId1, const IGsize ptId2) override;
    IGsize AddFace(igIndex* ptIds, int size) override;


    // Delete element, necessarily called after RequestEditStatus()
    void DeletePoint(const IGsize ptId) override;
    void DeleteEdge(const IGsize edgeId) override;
    void DeleteFace(const IGsize faceId) override;
    void DeleteVolume(const IGsize volumeId);

    CellArray* GetCells() { return this->GetVolumes(); };
    void GetCell(igIndex cellId, Cell* cell) {
        cell = this->GetVolume(cellId);
    };
    Cell* GetCell(igIndex cellId) {
        return this->GetVolume(cellId);
    }
    IntArray* GetCellTypes() {
        IntArray::Pointer Types = IntArray::New();
        Types->Resize(this->GetNumberOfVolumes());
        Types->SetElementSize(this->GetNumberOfVolumes());
        auto types = Types->RawPointer();
        igIndex cell[IGAME_CELL_MAX_SIZE];
        for (int i = 0; i < this->GetNumberOfVolumes(); i++) {
            int ncells = m_Volumes->GetCellIds(i, cell);
            switch (ncells) {
            case 4:
                types[i] = IG_TETRA;
                break;
            case 5:
                types[i] = IG_PYRAMID;
                break;
            case 6:
                types[i] = IG_PRISM;
                break;
            case 8:
                types[i] = IG_HEXAHEDRON;
                break;
            default:
                break;
            }
        }
        return Types.get();
    }
    igIndex GetCellDimension(igIndex CellTyp) { return 3; };

    bool IsBoundryVolume(igIndex VolumeId);
    bool IsBoundryFace(igIndex FaceId);
    bool IsBoundryEdge(igIndex EdgeId);
    bool IsBoundryPoint(igIndex PointId);
    bool IsCornerPoint(igIndex PointId);

    void InitPolyhedronVertices() {
        this->m_Volumes = CellArray::New();
        igIndex CellNum = this->m_VolumeFaces->GetNumberOfCells();
        igIndex i = 0, j = 0, k = 0;
        igIndex fhs[IGAME_CELL_MAX_SIZE] = { 0 };
        igIndex vhs[IGAME_CELL_MAX_SIZE] = { 0 };
        igIndex fvhs[IGAME_CELL_MAX_SIZE] = { 0 };
        igIndex fcnt = 0, vcnt = 0, fvcnt = 0;
        for (i = 0; i < CellNum; i++) {
            vcnt = 0;
            std::set<igIndex>vset;
            fcnt = this->m_VolumeFaces->GetCellIds(i, fhs);
            for (j = 0; j < fcnt; j++) {
                fvcnt = this->m_Faces->GetCellIds(fhs[j], fvhs);
                for (k = 0; k < fvcnt; k++) {
                    vset.insert(fvhs[k]);
                }
            }
            for (auto it : vset) {
                vhs[vcnt++] = it;
            }
            this->m_Volumes->AddCellIds(vhs, vcnt);
        }
    }
    void InitVolumesWithPolyhedron(CellArray::Pointer faces, CellArray::Pointer VolumeFaces)
    {
        m_VolumeFaces = VolumeFaces;
        m_Faces = faces;
        InitPolyhedronVertices();
        this->isPolyhedron = true;
    }

protected:
    VolumeMesh_2();
    ~VolumeMesh_2() override = default;

    void RequestFaceStatus();
    void RequestVolumeStatus();

    DeleteMarker::Pointer m_VolumeDeleteMarker{};

    CellArray::Pointer m_Volumes{};     // The point set of volumes
    CellLinks::Pointer m_VolumeLinks{}; // The adjacent volumes of points

    CellArray::Pointer m_VolumeEdges{};     // The edge set of volumes
    CellLinks::Pointer m_VolumeEdgeLinks{}; // The adjacent volumes of edges

    CellArray::Pointer m_VolumeFaces{};     // The face set of volumes
    CellLinks::Pointer m_VolumeFaceLinks{}; // The adjacent volumes of faces

    bool isPolyhedron = false;
private:
    Tetra::Pointer
        m_Tetra{}; // Used for the returned 'Tetra' object, which is Thread-Unsafe
    Hexahedron::Pointer
        m_Hexahedron{}; // Used for the returned 'Hexahedron' object, which is Thread-Unsafe
    Prism::Pointer
        m_Prism{}; // Used for the returned 'Prism' object, which is Thread-Unsafe
    Pyramid::Pointer
        m_Pyramid{}; // Used for the returned 'Pyramid' object, which is Thread-Unsafe

public:
    void Draw(Scene*) override;
    void ConvertToDrawableData() override;
    bool IsDrawable() override { return true; }
    void ViewCloudPicture(Scene* scene, int index, int demension = -1) override;

    void SetAttributeWithPointData(ArrayObject::Pointer attr,
        igIndex i = -1) override;

    void SetAttributeWithCellData(ArrayObject::Pointer attr, igIndex i = -1);

private:
    GLVertexArray m_PointVAO, m_LineVAO, m_TriangleVAO;
    GLBuffer m_PositionVBO, m_ColorVBO, m_NormalVBO, m_TextureVBO;
    GLBuffer m_PointEBO, m_LineEBO, m_TriangleEBO;

    GLVertexArray m_CellVAO;
    GLBuffer m_CellPositionVBO, m_CellColorVBO;

    FloatArray::Pointer m_Positions{};
    FloatArray::Pointer m_Colors{};
    IdArray::Pointer m_PointIndices{};
    IdArray::Pointer m_LineIndices{};
    IdArray::Pointer m_TriangleIndices{};

    bool m_Flag{ false };
    bool m_UseColor{ false };
    bool m_ColorWithCell{ false };
    int m_PointSize{ 8 };
    int m_LineWidth{ 1 };
    int m_CellPositionSize{};

    ArrayObject::Pointer m_ViewAttribute{};
    int m_ViewDemension{};
};
IGAME_NAMESPACE_END
#endif