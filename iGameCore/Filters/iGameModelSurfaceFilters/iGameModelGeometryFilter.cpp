#include "iGameModelGeometryFilter.h"
#include "iGameAtomicMutex.h"
#include "iGameThreadPool.h"
#include <mutex>
#include <omp.h>
IGAME_NAMESPACE_BEGIN
#define ArrayList std::vector<ArrayObject>
iGameModelGeometryFilter::iGameModelGeometryFilter() {
	this->PointMinimum = 0;
	this->PointMaximum = LLONG_MAX;

	this->CellMinimum = 0;
	this->CellMaximum = LLONG_MAX;

	this->Extent[0] = -DBL_MAX;
	this->Extent[1] = DBL_MAX;
	this->Extent[2] = -DBL_MAX;
	this->Extent[3] = DBL_MAX;
	this->Extent[4] = -DBL_MAX;
	this->Extent[5] = DBL_MAX;
	this->PointClipping = false;
	this->CellClipping = false;
	this->ExtentClipping = false;

	this->Merging = true;

	this->FastMode = false;
	this->RemoveGhostInterfaces = true;

	this->PieceInvariant = 0;

	this->PassThroughCellIds = 0;
	this->PassThroughPointIds = 0;
	this->OriginalCellIdsName = nullptr;
	this->OriginalPointIdsName = nullptr;


	// Compatibility with vtkDataSetSurfaceFilter
	this->NonlinearSubdivisionLevel = 1;

	// Enable delegation to an internal vtkDataSetSurfaceFilter.
	this->Delegation = true;
}
iGameModelGeometryFilter::~iGameModelGeometryFilter() {}
void iGameModelGeometryFilter::SetExtent(double xMin, double xMax, double yMin,
	double yMax, double zMin,
	double zMax) {
	double extent[6] = { xMin, xMax, yMin, yMax, zMin, zMax };
	this->SetExtent(extent);
}
void iGameModelGeometryFilter::SetExtent(double extent[6]) {
	int i;
	bool needSet = false;
	for (i = 0; i < 6; i++) { needSet |= extent[i] != this->Extent[i]; }
	if (needSet) {
		this->Modified();
		for (i = 0; i < 3; i++) {
			if (extent[2 * i + 1] < extent[2 * i]) {
				std::swap(extent[2 * i + 1], extent[2 * i]);
			}
			this->Extent[2 * i] = extent[2 * i];
			this->Extent[2 * i + 1] = extent[2 * i + 1];
		}
	}
}

bool iGameModelGeometryFilter::Execute() {

	Execute(this->input);
	return true;
}
bool iGameModelGeometryFilter::Execute(DataObject* input) {

	this->output = PolyData::New();
	return Execute(input, output);
}

bool iGameModelGeometryFilter::Execute(DataObject* input, PolyData* output) {

	switch (input->GetDataObjectType())
	{
	case IG_NONE:
		return true;
	case IG_VOLUME_MESH:
		return true;
	case IG_SURFACE_MESH:
		return true;
	case IG_UNSTRUCTURED_MESH:
		return this->ExecuteWithUnstructuredGrid(input, output, excFaces);
	case IG_STRUCTURED_MESH:
		return this->ExecuteWithStructuredGrid(input, output, excFaces);
	default:
		break;
	}

	//if (DynamicCast<PolyData>(input)) {
	//    return this->ExecuteWithPolyData(input, output, excFaces);
	//} else if (DynamicCast<UnstructuredMesh>(input)) {
	//    return this->ExecuteWithUnstructuredGrid(input, output, excFaces);
	//} else if (auto structuredGrid = DynamicCast<StructuredGrid>(input)) {
	//    auto Dimension = structuredGrid->GetDimension();
	//    if (Dimension == 3 && !this->CellClipping && !this->PointClipping &&
	//        !this->ExtentClipping) {
	//        return this->ExecuteWithStructuredGrid(input, output, excFaces);
	//    }
	//}
	////还有其他种类，可能是普通的dataobject存储数据这边判断下走最普通的dataset执行
	//else {
	//    igError("Data object " + input->GetName() + "is not supported.");
	//    return 0;
	//}
	// Use the general case
	return this->ExecuteWithDataSet(input, output, excFaces);
}

class GFace {
public:
	GFace* Next = nullptr;
	//父亲cell，用于对cell attribute的map
	igIndex OriginalCellId;
	igIndex* PointIds;
	int NumberOfPoints;
	//是否是幽灵面
	bool IsGhost;

	GFace() = default;
	GFace(const igIndex& originalCellId, const igIndex& numberOfPoints,
		const bool& isGhost)
		: OriginalCellId(static_cast<igIndex>(originalCellId)),
		NumberOfPoints(numberOfPoints), IsGhost(isGhost) {}

	bool operator==(const GFace& other) const {
		if (this->NumberOfPoints != other.NumberOfPoints) { return false; }
		switch (this->NumberOfPoints) {
		case 3: {
			return this->PointIds[0] == other.PointIds[0] &&
				((this->PointIds[1] == other.PointIds[2] &&
					this->PointIds[2] == other.PointIds[1]) ||
					(this->PointIds[1] == other.PointIds[1] &&
						this->PointIds[2] == other.PointIds[2]));
		}
		case 4: {
			return this->PointIds[0] == other.PointIds[0] &&
				this->PointIds[2] == other.PointIds[2] &&
				((this->PointIds[1] == other.PointIds[3] &&
					this->PointIds[3] == other.PointIds[1]) ||
					(this->PointIds[1] == other.PointIds[1] &&
						this->PointIds[3] == other.PointIds[3]));
		}
		default: {
			bool match = true;
			if (this->PointIds[0] == other.PointIds[0]) {
				// if the first two points match loop through forwards
				// checking all points
				if (this->NumberOfPoints > 1 &&
					this->PointIds[1] == other.PointIds[1]) {
					for (auto i = 2; i < this->NumberOfPoints; ++i) {
						if (this->PointIds[i] != other.PointIds[i]) {
							match = false;
							break;
						}
					}
				}
				else {
					// check if the points go in the opposite direction
					for (auto i = 1; i < this->NumberOfPoints; ++i) {
						if (this->PointIds[this->NumberOfPoints - i] !=
							other.PointIds[i]) {
							match = false;
							break;
						}
					}
				}
			}
			else {
				match = false;
			}
			return match;
		}
		}
	}
	bool operator!=(const GFace& other) const { return !(*this == other); }
};

/**
 * A subclass of GFace to define Faces with a static number of points
 */
template<int Fcnt>
class StaticFace : public GFace {
private:
	std::array<igIndex, Fcnt> PointIdsContainer{};

public:
	StaticFace(const igIndex& originalCellId, const igIndex* pointIds,
		const bool& isGhost)
		: GFace(originalCellId, Fcnt, isGhost) {
		this->PointIds = this->PointIdsContainer.data();
		this->Initialize(pointIds);
	}

	inline static constexpr int GetSize() { return Fcnt; }

	void Initialize(const igIndex* pointIds) {
		// find the index to the smallest id
		int offset = 0;
		int index;
		for (index = 1; index < Fcnt; ++index) {
			if (pointIds[index] < pointIds[offset]) { offset = index; }
		}
		// copy ids into ordered array with the smallest id first
		for (index = 0; index < Fcnt; ++index) {
			this->PointIds[index] =
				static_cast<igIndex>(pointIds[(offset + index) % Fcnt]);
		}
	}
};

/**
 * A subclass of GFace to define Faces with a dynamic number of points
 */
class DynamicFace : public GFace {
private:
	std::vector<igIndex> PointIdsContainer;

public:
	DynamicFace(const igIndex& originalCellId, const igIndex& numberOfPoints,
		const igIndex* pointIds, const bool& isGhost)
		: GFace(originalCellId, numberOfPoints, isGhost) {
		assert(this->NumberOfPoints != 0);
		this->PointIdsContainer.resize(
			static_cast<size_t>(this->NumberOfPoints));
		this->PointIds = this->PointIdsContainer.data();
		this->Initialize(pointIds);
	}

	inline int GetSize() const { return this->NumberOfPoints; }

	void Initialize(const igIndex* pointIds) {
		// find the index to the smallest id
		int offset = 0;
		int index;
		for (index = 1; index < this->NumberOfPoints; ++index) {
			if (pointIds[index] < pointIds[offset]) { offset = index; }
		}
		// copy ids into ordered array with the smallest id first
		for (index = 0; index < this->NumberOfPoints; ++index) {
			this->PointIds[index] = static_cast<igIndex>(
				pointIds[(offset + index) % this->NumberOfPoints]);
		}
	}
};


using GTriangle = StaticFace<3>;

using GQuad = StaticFace<4>;

using GPentagon = StaticFace<5>;

using GHexagon = StaticFace<6>;

using GHeptagon = StaticFace<7>;

using GOctagon = StaticFace<8>;

using GNonagon = StaticFace<9>;

using GDecagon = StaticFace<10>;

using GPolygon = DynamicFace;

class FaceMemoryPool {
private:
	igIndex NumberOfArrays;
	igIndex ArrayLength;
	igIndex NextArrayIndex;
	igIndex NextFaceIndex;
	unsigned char** Arrays;
	inline static int SizeofFace(const int& numberOfPoints) {
		static constexpr int fSize = sizeof(GFace);
		static constexpr int sizeId = sizeof(igIndex);
		if (fSize % sizeId == 0) {
			return static_cast<int>(fSize + numberOfPoints * sizeId);
		}
		else {
			return static_cast<int>((fSize / sizeId + 1 + numberOfPoints) *
				sizeId);
		}
	}

public:
	FaceMemoryPool()
		: NumberOfArrays(0), ArrayLength(0), NextArrayIndex(0),
		NextFaceIndex(0),
		Arrays(nullptr) /*, Lock(std::make_unique<std::mutex>()) */ {}

	~FaceMemoryPool() { this->Destroy(); }

	void Initialize(const igIndex& numberOfPoints) {
		this->Destroy();
		this->NumberOfArrays = 100;
		this->NextArrayIndex = 0;
		this->NextFaceIndex = 0;
		this->Arrays = new unsigned char* [this->NumberOfArrays];
		for (auto i = 0; i < this->NumberOfArrays; i++) {
			this->Arrays[i] = nullptr;
		}
		// size the chunks based on the size of a quadrilateral
		int quadSize = SizeofFace(4);
		if (numberOfPoints < this->NumberOfArrays) {
			this->ArrayLength = 50 * quadSize;
		}
		else {
			this->ArrayLength = (numberOfPoints / 2) * quadSize;
		}
	}

	void Destroy() {
		for (auto i = 0; i < this->NumberOfArrays; i++) {
			delete[] this->Arrays[i];
			this->Arrays[i] = nullptr;
		}
		delete[] this->Arrays;
		this->Arrays = nullptr;
		this->ArrayLength = 0;
		this->NumberOfArrays = 0;
		this->NextArrayIndex = 0;
		this->NextFaceIndex = 0;
	}

	GFace* Allocate(const int& numberOfPoints) {
		// see if there's room for this one
		const int polySize = SizeofFace(numberOfPoints);
		//std::cout << this->NextArrayIndex << " " << this->NextFaceIndex << '\n';
		if (this->NextFaceIndex + polySize > this->ArrayLength) {
			++this->NextArrayIndex;
			this->NextFaceIndex = 0;
		}

		// Although this should not happen often, check first.
		if (this->NextArrayIndex >= this->NumberOfArrays) {
			int idx, num;
			unsigned char** newArrays;
			num = this->NumberOfArrays * 2;
			newArrays = new unsigned char* [num];
			for (idx = 0; idx < num; ++idx) {
				newArrays[idx] = nullptr;
				if (idx < this->NumberOfArrays) {
					newArrays[idx] = this->Arrays[idx];
				}
			}
			delete[] this->Arrays;
			this->Arrays = newArrays;
			this->NumberOfArrays = num;
		}

		// Next: allocate a new array if necessary.
		if (this->Arrays[this->NextArrayIndex] == nullptr) {
			this->Arrays[this->NextArrayIndex] =
				new unsigned char[this->ArrayLength];
		}

		GFace* Face = reinterpret_cast<GFace*>(
			this->Arrays[this->NextArrayIndex] + this->NextFaceIndex);
		Face->NumberOfPoints = numberOfPoints;

		static constexpr int fSize = sizeof(GFace);
		static constexpr int sizeId = sizeof(igIndex);
		// If necessary, we create padding after TFace such that
		// the beginning of ids aligns evenly with sizeof(TInputIdType).
		if (fSize % sizeId == 0) {
			Face->PointIds = (igIndex*)Face + fSize / sizeId;
		}
		else {
			Face->PointIds = (igIndex*)Face + fSize / sizeId + 1;
		}

		this->NextFaceIndex += polySize;

		return Face;
	}
};
class CellArrayType {
private:
	igIndex* PointMap;
	//vtkStaticCellLinksTemplate<igIndex>* ExcFaces;
	const unsigned char* PointGhost;

public:
	// Make things a little more expressive
	using IdListType = std::vector<igIndex>;
	IdListType Cells;
	IdListType OrigCellIds;

	CellArrayType() : PointMap(nullptr), PointGhost(nullptr) {}

	void SetPointsGhost(const unsigned char* pointGhost) {
		this->PointGhost = pointGhost;
	}
	void SetPointMap(igIndex* ptMap) { this->PointMap = ptMap; }
	//void SetExcludedFaces(vtkStaticCellLinksTemplate<TInputIdType>* exc) {
	//    this->ExcFaces = exc;
	//}
	igIndex GetNumberOfCells() {
		return static_cast<igIndex>(this->OrigCellIds.size());
	}
	igIndex GetNumberOfConnEntries() {
		return static_cast<igIndex>(this->Cells.size());
	}
	static constexpr unsigned char MASKED_POINT_VALUE = 2;

	void InsertNextCell(igIndex npts, const igIndex* pts, igIndex cellId) {
		if (this->PointGhost) {
			for (auto i = 0; i < npts; ++i) {
				if (this->PointGhost[pts[i]] & MASKED_POINT_VALUE) { return; }
			}
		}
		this->Cells.emplace_back(npts);
		if (!this->PointMap) {
			for (auto i = 0; i < npts; ++i) {
				this->Cells.emplace_back(static_cast<igIndex>(pts[i]));
			}
		}
		else {
			for (auto i = 0; i < npts; ++i) {
				this->Cells.emplace_back(static_cast<igIndex>(pts[i]));
				this->PointMap[pts[i]] = 1;
			}
		}
		this->OrigCellIds.emplace_back(static_cast<igIndex>(cellId));
	}
};
class FaceHashMap {
private:
	using TCellArrayType = CellArrayType;
	using TFaceMemoryPool = FaceMemoryPool;
	struct Bucket {
		GFace* Head;
		iGameAtomicMutex Lock;
		Bucket() : Head(nullptr) {}
	};
	size_t Size;
	std::vector<Bucket> Buckets;

public:
	FaceHashMap(const size_t& size) : Size(size) {
		this->Buckets.resize(this->Size);
	}
	std::vector<Bucket>& GetBuckets() { return this->Buckets; }
	//插入面到池子中，如果已经存在就去除，如果不存在就加入
	template<typename TypeFace>
	void Insert(const TypeFace& f, TFaceMemoryPool* pool) {
		const size_t key = static_cast<size_t>(f.PointIds[0]) % this->Size;
		auto& bucket = this->Buckets[key];
		auto& bucketHead = bucket.Head;
		std::lock_guard<iGameAtomicMutex> lock(bucket.Lock);
		auto current = bucketHead;
		auto previous = current;
		while (current != nullptr) {
			if (*current == f) {
				// delete the duplicate
				if (bucketHead == current) {
					bucketHead = current->Next;
				}
				else {
					previous->Next = current->Next;
				}
				return;
			}
			previous = current;
			current = current->Next;
		}
		//not existed, allocate a new face
		//GFace* newF = new GFace(f.OriginalCellId, f.GetSize(), f.IsGhost);
		//newF->PointIds = new igIndex[f.GetSize()];
		GFace* newF = pool->Allocate(f.GetSize());
		newF->Next = nullptr;
		newF->OriginalCellId = f.OriginalCellId;
		newF->IsGhost = f.IsGhost;
		std::copy(f.PointIds, f.PointIds + f.GetSize(), newF->PointIds);
		if (bucketHead == nullptr) {
			bucketHead = newF;
		}
		else {
			previous->Next = newF;
		}
	}

	void PopulateCellArrays(std::vector<TCellArrayType*>& threadedPolys) {
		std::vector<GFace*> Faces;
		for (auto& bucket : this->Buckets) {
			if (bucket.Head != nullptr) {
				auto current = bucket.Head;
				while (current != nullptr) {
					if (!current->IsGhost) { Faces.push_back(current); }
					current = current->Next;
				}
			}
		}
		const igIndex numberOfThreads =
			static_cast<igIndex>(threadedPolys.size());
		const igIndex numberOfFaces = static_cast<igIndex>(Faces.size());
		for (igIndex threadId = 0; threadId < numberOfThreads; ++threadId) {
			igIndex begin = threadId * numberOfFaces / numberOfThreads;
			igIndex end = (threadId + 1) * numberOfFaces / numberOfThreads;
			for (igIndex i = begin; i < end; ++i) {
				auto& f = Faces[i];
				threadedPolys[threadId]->InsertNextCell(
					f->NumberOfPoints, f->PointIds, f->OriginalCellId);
			}
		}
	}
};


void ExtractCellGeometry(UnstructuredMesh::Pointer input, igIndex cellId, int cellType,
	igIndex npts, const igIndex* pts,
	FaceMemoryPool* GFacePool, FaceHashMap* GFaceMap,
	const bool& isGhost) {

	int GFaceId, numFaces, numGFacePts;
	igIndex ptIds[IGAME_CELL_MAX_SIZE]; // cell GFace point ids
	igIndex Ids[IGAME_CELL_MAX_SIZE];
	const igIndex* GFaceVerts;
	static constexpr int pixelConvert[4] = { 0, 1, 3, 2 };
	switch (cellType) {
	case IG_EMPTY_CELL:
		break;

	case IG_VERTEX:
		//case IG_POLY_VERTEX:
		//verts.InsertNextCell(npts, pts, cellId);
		break;

	case IG_LINE:
		//case IG_POLY_LINE:
		//    lines.InsertNextCell(npts, pts, cellId);
		break;

	case IG_TRIANGLE:
	case IG_QUAD:
	case IG_POLYGON:
		//polys.InsertNextCell(npts, pts, cellId);
		break;

		//case IG_TRIANGLE_STRIP:
		//    strips.InsertNextCell(npts, pts, cellId);
		//break;

		//case IG_PIXEL:
		//    // pixelConvert (in the following loop) is an int[4]. GCC 5.1.1
		//    // warns about pixelConvert[4] being uninitialized due to loop
		//    // unrolling -- forcibly restricting npts <= 4 prevents this warning.
		//    ptIds[0] = pts[pixelConvert[0]];
		//    ptIds[1] = pts[pixelConvert[1]];
		//    ptIds[2] = pts[pixelConvert[2]];
		//    ptIds[3] = pts[pixelConvert[3]];
		//    polys.InsertNextCell(npts, ptIds, cellId);
		//    break;

	case IG_TETRA:
		for (GFaceId = 0; GFaceId < 4; GFaceId++) {
			GFaceVerts = Tetra::faces[GFaceId];
			ptIds[0] = pts[GFaceVerts[0]];
			ptIds[1] = pts[GFaceVerts[1]];
			ptIds[2] = pts[GFaceVerts[2]];
			GFaceMap->Insert(GTriangle(cellId, ptIds, isGhost), GFacePool);
		}
		break;

		//case IG_VOXEL:
		//    for (GFaceId = 0; GFaceId < 6; GFaceId++) {
		//        GFaceVerts = vtkVoxel::GeTFaceArray(GFaceId);
		//        ptIds[0] = pts[GFaceVerts[pixelConvert[0]]];
		//        ptIds[1] = pts[GFaceVerts[pixelConvert[1]]];
		//        ptIds[2] = pts[GFaceVerts[pixelConvert[2]]];
		//        ptIds[3] = pts[GFaceVerts[pixelConvert[3]]];
		//        GFaceMap->Insert(Quad<TInputIdType>(cellId, ptIds, isGhost),
		//                        localData->GFacePool);
		//    }
		//    break;

	case IG_HEXAHEDRON:
		for (GFaceId = 0; GFaceId < 6; GFaceId++) {
			GFaceVerts = Hexahedron::faces[GFaceId];
			ptIds[0] = pts[GFaceVerts[0]];
			ptIds[1] = pts[GFaceVerts[1]];
			ptIds[2] = pts[GFaceVerts[2]];
			ptIds[3] = pts[GFaceVerts[3]];
			GFaceMap->Insert(GQuad(cellId, ptIds, isGhost), GFacePool);
		}
		break;

	case IG_PRISM:
		for (GFaceId = 0; GFaceId < 5; GFaceId++) {
			GFaceVerts = Prism::faces[GFaceId];
			ptIds[0] = pts[GFaceVerts[0]];
			ptIds[1] = pts[GFaceVerts[1]];
			ptIds[2] = pts[GFaceVerts[2]];
			if (GFaceVerts[3] < 0) {
				GFaceMap->Insert(GTriangle(cellId, ptIds, isGhost),
					GFacePool);
			}
			else {
				ptIds[3] = pts[GFaceVerts[3]];
				GFaceMap->Insert(GQuad(cellId, ptIds, isGhost), GFacePool);
			}
		}
		break;

	case IG_PYRAMID:
		for (GFaceId = 0; GFaceId < 5; GFaceId++) {
			GFaceVerts = Pyramid::faces[GFaceId];
			ptIds[0] = pts[GFaceVerts[0]];
			ptIds[1] = pts[GFaceVerts[1]];
			ptIds[2] = pts[GFaceVerts[2]];
			if (GFaceVerts[3] < 0) {
				GFaceMap->Insert(GTriangle(cellId, ptIds, isGhost),
					GFacePool);
			}
			else {
				ptIds[3] = pts[GFaceVerts[3]];
				GFaceMap->Insert(GQuad(cellId, ptIds, isGhost), GFacePool);
			}
		}
		break;

		//case IG_HEXAGONAL_PRISM:
		//    for (GFaceId = 0; GFaceId < 8; GFaceId++) {
		//        GFaceVerts = vtkHexagonalPrism::GeTFaceArray(GFaceId);
		//        ptIds[0] = pts[GFaceVerts[0]];
		//        ptIds[1] = pts[GFaceVerts[1]];
		//        ptIds[2] = pts[GFaceVerts[2]];
		//        ptIds[3] = pts[GFaceVerts[3]];
		//        if (GFaceVerts[4] < 0) {
		//            GFaceMap->Insert(Quad<TInputIdType>(cellId, ptIds, isGhost),
		//                            localData->GFacePool);
		//        } else {
		//            ptIds[4] = pts[GFaceVerts[4]];
		//            ptIds[5] = pts[GFaceVerts[5]];
		//            GFaceMap->Insert(
		//                    Hexagon<TInputIdType>(cellId, ptIds, isGhost),
		//                    localData->GFacePool);
		//        }
		//    }
		//    break;

		//case IG_PENTAGONAL_PRISM:
		//    for (GFaceId = 0; GFaceId < 7; GFaceId++) {
		//        GFaceVerts = vtkPentagonalPrism::GeTFaceArray(GFaceId);
		//        ptIds[0] = pts[GFaceVerts[0]];
		//        ptIds[1] = pts[GFaceVerts[1]];
		//        ptIds[2] = pts[GFaceVerts[2]];
		//        ptIds[3] = pts[GFaceVerts[3]];
		//        if (GFaceVerts[4] < 0) {
		//            GFaceMap->Insert(Quad<TInputIdType>(cellId, ptIds, isGhost),
		//                            localData->GFacePool);
		//        } else {
		//            ptIds[4] = pts[GFaceVerts[4]];
		//            GFaceMap->Insert(
		//                    Pentagon<TInputIdType>(cellId, ptIds, isGhost),
		//                    localData->GFacePool);
		//        }
		//    }
		//    break;

	case IG_POLYHEDRON: {
		input->GetCellPointIds(cellId, Ids);
		igIndex index = 0;
		numFaces = Ids[index++];
		for (GFaceId = 0; GFaceId < numFaces; GFaceId++) {
			numGFacePts = Ids[index++];
			pts = Ids + index;
			index += numGFacePts;
			switch (numGFacePts) {
			case 3:
				GFaceMap->Insert(GTriangle(cellId, pts, isGhost),
					GFacePool);
				break;
			case 4:
				GFaceMap->Insert(GQuad(cellId, pts, isGhost),
					GFacePool);
				break;
			case 5:
				GFaceMap->Insert(GPentagon(cellId, pts, isGhost),
					GFacePool);
				break;
			case 6:
				GFaceMap->Insert(GHexagon(cellId, pts, isGhost),
					GFacePool);
				break;
			case 7:
				GFaceMap->Insert(GHeptagon(cellId, pts, isGhost),
					GFacePool);
				break;
			case 8:
				GFaceMap->Insert(GOctagon(cellId, pts, isGhost),
					GFacePool);
				break;
			case 9:
				GFaceMap->Insert(GNonagon(cellId, pts, isGhost),
					GFacePool);
				break;
			case 10:
				GFaceMap->Insert(GDecagon(cellId, pts, isGhost),
					GFacePool);
				break;
			default:
				GFaceMap->Insert(
					GPolygon(cellId, numGFacePts, pts, isGhost),
					GFacePool);
				break;
			}
		}
	}

					  break;

	default:
		//一般为多面体，需要通过cell找到面片
		Cell* cell = input->GetCell(cellId);
		auto cellType = input->GetCellType(cellId);
		if (/*Cell::GetCellDimension(cellType) == */ 3) {
			for (GFaceId = 0, numFaces = cell->GetNumberOfFaces();
				GFaceId < numFaces; GFaceId++) {
				Cell* GFace = cell->GetFace(GFaceId);
				numGFacePts =
					static_cast<int>(GFace->PointIds->GetNumberOfIds());
				switch (numGFacePts) {
				case 3:
					GFaceMap->Insert(
						GTriangle(cellId,
							GFace->PointIds->RawPointer(),
							isGhost),
						GFacePool);
					break;
				case 4:
					GFaceMap->Insert(
						GQuad(cellId, GFace->PointIds->RawPointer(),
							isGhost),
						GFacePool);
					break;
				case 5:
					GFaceMap->Insert(
						GPentagon(cellId,
							GFace->PointIds->RawPointer(),
							isGhost),
						GFacePool);
					break;
				case 6:
					GFaceMap->Insert(
						GHexagon(cellId,
							GFace->PointIds->RawPointer(),
							isGhost),
						GFacePool);
					break;
				case 7:
					GFaceMap->Insert(
						GHeptagon(cellId,
							GFace->PointIds->RawPointer(),
							isGhost),
						GFacePool);
					break;
				case 8:
					GFaceMap->Insert(
						GOctagon(cellId,
							GFace->PointIds->RawPointer(),
							isGhost),
						GFacePool);
					break;
				case 9:
					GFaceMap->Insert(
						GNonagon(cellId,
							GFace->PointIds->RawPointer(),
							isGhost),
						GFacePool);
					break;
				case 10:
					GFaceMap->Insert(
						GDecagon(cellId,
							GFace->PointIds->RawPointer(),
							isGhost),
						GFacePool);
					break;
				default:
					GFaceMap->Insert(
						GPolygon(cellId, numGFacePts,
							GFace->PointIds->RawPointer(),
							isGhost),
						GFacePool);
					break;
				}
			} // for all cell Faces
		}     // if 3D
		else {
			igDebug("Unknown cell type.");
		}
	} // switch
} // ExtractCellGeometry()

void ExtractCellGeometry(VolumeMesh::Pointer input, igIndex cellId,
	igIndex npts, const igIndex* pts,
	FaceMemoryPool* GFacePool, FaceHashMap* GFaceMap,
	const bool& isGhost) {

	int GFaceId, numFaces, numGFacePts;
	igIndex ptIds[IGAME_CELL_MAX_SIZE]; // cell GFace point ids
	igIndex Ids[IGAME_CELL_MAX_SIZE];
	const igIndex* GFaceVerts;
	static constexpr int pixelConvert[4] = { 0, 1, 3, 2 };
	switch (npts)
	{
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	default:
		break;
	}
	
} // ExtractCellGeometry()

struct ExtractCellBoundaries {
	// If point merging is specified, then a point map is created.
	igIndex* PointMap;

	// Cell visibility and cell ghost levels
	const char* CellVis;
	const unsigned char* CellGhosts;
	const unsigned char* PointGhost;
	// Thread-related output data

	igIndex PolysNumPts, PolysNumCells;
	igIndex PolysCellIdOffset;
	igIndex NumPts;
	igIndex NumCells;
	ExtractCellBoundaries* Extract;
	//vtkStaticCellLinksTemplate<TInputIdType>* ExcFaces;
	//using TThreadOutputType = ThreadOutputType<TInputIdType>;
	//TThreadOutputType* Threads;

	ExtractCellBoundaries(const char* cellVis, const unsigned char* cellGhosts,
		const unsigned char* pointGhost /*,
	   vtkExcludedFaces<TInputIdType>* exc,
		TThreadOutputType* threads*/)
		: PointMap(nullptr), CellVis(cellVis), CellGhosts(cellGhosts),
		PointGhost(pointGhost)/*, Threads(threads)*/ {
		/*this->ExcFaces = (exc == nullptr ? nullptr : exc->Links);*/
	}

	virtual ~ExtractCellBoundaries() { delete[] this->PointMap; }

	// If point merging is needed, create the point map (map from old points
	// to new points).
	void CreatePointMap(igIndex numPts) {
		this->PointMap = new igIndex[numPts];
		std::fill(this->PointMap, this->PointMap + numPts, -1);
	}

	// Initialize thread data
	virtual void Initialize() {}


};

int iGameModelGeometryFilter::ExecuteWithPolyData(DataObject::Pointer input,
	PolyData::Pointer output,
	PolyData::Pointer  exc) {
	return 1;
}
int iGameModelGeometryFilter::ExecuteWithPolyData(DataObject::Pointer input,
	PolyData::Pointer  output) {
	return this->ExecuteWithPolyData(input, output, nullptr);
}


struct ExtractVM : public ExtractCellBoundaries {
	// The unstructured grid to process
	VolumeMesh::Pointer Grid;
	std::shared_ptr<FaceHashMap> FaceMap;
	bool RemoveGhostInterFaces;

	ExtractVM(VolumeMesh::Pointer grid, const char* cellVis,
		const unsigned char* cellGhost, const unsigned char* pointGhost,
		bool merging, bool removeGhostInterFaces)
		: ExtractCellBoundaries(cellVis, cellGhost, pointGhost), Grid(grid),
		RemoveGhostInterFaces(removeGhostInterFaces) {
		if (merging) { this->CreatePointMap(grid->GetNumberOfPoints()); }
		this->FaceMap = std::make_shared<FaceHashMap>(
			static_cast<size_t>(grid->GetNumberOfPoints()));
		this->Initialize();
	}

	// Initialize thread data
	void Initialize() override {
		this->ExtractCellBoundaries::Initialize();
	}

	void Execute(igIndex beginCellId, igIndex endCellId,
		FaceMemoryPool* GFacePool) {
		igIndex cellId;
		bool isGhost = false;
		igIndex pts[IGAME_CELL_MAX_SIZE];
		igIndex npts = 0;
		auto GFaceMap = this->FaceMap.get();
		if (this->Grid) {
			for (cellId = beginCellId; cellId < endCellId; cellId++) {
				if (isGhost ) {
					continue;
				}
				// If the cell is visible process it
				if (!this->CellVis || this->CellVis[cellId]) {
					ExtractCellGeometry(this->Grid, cellId, npts, pts,
						GFacePool, GFaceMap, isGhost);
				}

			}
		}
	} // operator()


};

int iGameModelGeometryFilter::ExecuteWithVolumeMesh(
	DataObject::Pointer input, PolyData::Pointer output, PolyData::Pointer exc) {

	return true;
}
int iGameModelGeometryFilter::ExecuteWithVolumeMesh(DataObject::Pointer input,
	PolyData::Pointer output)
{
	return  ExecuteWithVolumeMesh(input, output, nullptr);
}
struct ExtractUG : public ExtractCellBoundaries {
	// The unstructured grid to process
	UnstructuredMesh::Pointer Grid;
	std::shared_ptr<FaceHashMap> FaceMap;
	bool RemoveGhostInterFaces;

	ExtractUG(UnstructuredMesh* grid, const char* cellVis,
		const unsigned char* cellGhost, const unsigned char* pointGhost,
		bool merging, bool removeGhostInterFaces)
		: ExtractCellBoundaries(cellVis, cellGhost, pointGhost), Grid(grid),
		RemoveGhostInterFaces(removeGhostInterFaces) {
		if (merging) { this->CreatePointMap(grid->GetNumberOfPoints()); }
		this->FaceMap = std::make_shared<FaceHashMap>(
			static_cast<size_t>(grid->GetNumberOfPoints()));
		this->Initialize();
	}

	// Initialize thread data
	void Initialize() override {
		this->ExtractCellBoundaries::Initialize();
	}

	void Execute(igIndex beginCellId, igIndex endCellId,
		FaceMemoryPool* GFacePool) {
		igIndex cellId;
		bool isGhost = false;
		igIndex pts[IGAME_CELL_MAX_SIZE];
		igIndex npts = 0;
		auto GFaceMap = this->FaceMap.get();
		if (this->Grid) {
			auto cellTypes = Grid->GetCellTypes()->RawPointer();
			for (cellId = beginCellId; cellId < endCellId; cellId++) {
				igIndex cellType = cellTypes[cellId];
				//如果是虚拟Cell
				if (isGhost && (Cell::GetCellDimension(cellType) < 3 ||
					!this->RemoveGhostInterFaces)) {
					continue;
				}
				// If the cell is visible process it
				if (!this->CellVis || this->CellVis[cellId]) {
					Grid->GetCellPointIds(cellId, pts);
					ExtractCellGeometry(this->Grid, cellId, cellType, npts, pts,
						GFacePool, GFaceMap, isGhost);
				} // if cell visible
			}
		}
	} // operator()


};
int iGameModelGeometryFilter::ExecuteWithUnstructuredGrid(
	DataObject::Pointer input, PolyData::Pointer output, PolyData::Pointer exc) {
	clock_t time1 = clock();
	UnstructuredMesh* Grid = DynamicCast<UnstructuredMesh>(input);
	igDebug("Input has " << Grid->GetNumberOfPoints() << " points and "
		<< Grid->GetNumberOfCells() << " cells.");
	//if (Grid->GetNumberOfCells() == 0) {
	//    igDebug(this, "This unstructured grid doesn't have cell.");
	//}
	igIndex i = 0, j = 0, k = 0;
	igIndex64 cellId = 0, pointId = 0;
	igIndex64 numCells = Grid->GetNumberOfCells();
	igIndex64 numInputPts = Grid->GetNumberOfPoints();
	igIndex64 numOutputPts = 0;
	auto inPoints = Grid->GetPoints();
	auto inAllDataArray = input->GetAttributeSet();
	auto outAllDataArray = AttributeSet::New();
	StringArray::Pointer attrbNameArray = StringArray::New();
	CellArray::Pointer Polygons = CellArray::New();
	CharArray::Pointer CellVisibleArray = CharArray::New();
	char* CellVisible = nullptr;
	unsigned char* cellGhosts = nullptr;
	unsigned char* pointGhosts = nullptr;

	// Determine nature of what we have to do
	if ((!CellClipping) && (!PointClipping) && (!ExtentClipping)) {
		CellVisible = nullptr;
	}
	else {
		CellVisibleArray->Resize(numCells);
		CellVisible = CellVisibleArray->RawPointer();
	}
	// Mark cells as being visible or not
	//
	if (CellVisible) {
		CellArray::Pointer Polygons;
		igIndex vhs[256] = { 0 };
		igIndex vnum = 0;
		Point x;
		for (cellId = 0; cellId < numCells; cellId++) {
			CellVisible[cellId] = 1;
			if (CellClipping &&
				(cellId < CellMinimum || cellId > CellMaximum)) {
				CellVisible[cellId] = 0;
			}
			else {
				vnum = Grid->GetCellPointIds(cellId, vhs);
				for (i = 0; i < vnum; i++) {
					pointId = vhs[i];
					x = inPoints->GetPoint(i);
					if ((PointClipping &&
						(pointId < PointMinimum || pointId > PointMaximum)) ||
						(ExtentClipping &&
							(x[0] < Extent[0] || x[0] > Extent[1] ||
								x[1] < Extent[2] || x[1] > Extent[3] ||
								x[2] < Extent[4] || x[2] > Extent[5]))) {
						CellVisible[cellId] = 0;
						break;
					}
				}
			}
		}
	}


	auto* extract = new ExtractUG(Grid, CellVisible, cellGhosts, pointGhosts,
		this->Merging, this->RemoveGhostInterfaces);
	igIndex threadSize = 12;
	auto func = [&](int i) -> void {
		int st = i * numCells / threadSize;
		int ed = (i + 1) * numCells / threadSize;
		FaceMemoryPool* GFacePool = new FaceMemoryPool;
		GFacePool->Initialize(Grid->GetNumberOfPoints() / threadSize);
		extract->Execute(st, ed, GFacePool);
	};
	std::vector<std::thread> threads;
	for (int i = 0; i < threadSize; i++) {
		threads.emplace_back(std::thread(func, i));
	}
	for (int i = 0; i < threadSize; i++) { threads[i].join(); }

	numCells = extract->NumCells;

	clock_t time_2 = clock();
	igDebug("Extracted surface(not composite) cost " << time_2 - time1 << "ms.");

	auto& buckets = extract->FaceMap.get()->GetBuckets();
	std::vector<igIndex> f2c;
	for (i = 0; i < numInputPts; i++) {
		auto current = buckets[i].Head;
		while (current != nullptr) {
			Polygons->AddCellIds(current->PointIds, current->NumberOfPoints);
			f2c.emplace_back(current->OriginalCellId);
			current = current->Next;
		}
	}

	CompositeAttribute(f2c, inAllDataArray, outAllDataArray);
	for (i = 0; i < outAllDataArray->GetAllAttributes().GetPointer()->Size(); i++) {
		attrbNameArray->AddElement(
			outAllDataArray->GetAttribute(i).pointer.get()->GetName());
	}
	output->SetPoints(inPoints);
	output->SetFaces(Polygons);
	output->SetAttributeSet(outAllDataArray);
	output->GetMetadata()->AddStringArray(ATTRIBUTE_NAME_ARRAY, attrbNameArray);

	igDebug("Extracted " << output->GetNumberOfPoints() << " points,"
		<< output->GetNumberOfCells() << " cells.");
	f2c.swap(std::vector<igIndex>());
	delete extract;
	clock_t time2 = clock();
	igDebug("Extracted surface cost " << time2 - time1 << "ms.");
	return 1;
}
void iGameModelGeometryFilter::CompositeAttribute(std::vector<igIndex>& F2C,
	AttributeSet* inAllDataArray, AttributeSet* outAllDataArray) {

	igIndex i = 0, j = 0;
	auto inDataArrayNum = inAllDataArray->GetAllAttributes()->GetNumberOfElements();
	std::vector<AttributeSet::Attribute> CellArrays;
	for (i = 0; i < inDataArrayNum; i++) {
		if (inAllDataArray->GetAttribute(i).attachmentType == IG_CELL) {
			CellArrays.emplace_back(inAllDataArray->GetAttribute(i));
		}
		else {
			outAllDataArray->AddAttribute(inAllDataArray->GetAttribute(i).type, IG_POINT, inAllDataArray->GetAttribute(i).pointer);
		}
	}
	iGameAtomicMutex tmpLock;
	igIndex threadSize = 12;
	IGsize fcnt = F2C.size();
	auto f2c = F2C.data();
	auto func = [&](int i) -> void {
		for (igIndex CellArrayID = i; CellArrayID < CellArrays.size();
			CellArrayID += threadSize) {
			auto& inData = CellArrays[CellArrayID].pointer;
			double tmp[64];
			auto newData = DoubleArray::New();
			newData->SetElementSize(inData->GetElementSize());
			newData->Reserve(fcnt);
			for (j = 0; j < fcnt; j++) {
				inData->GetElement(f2c[j], tmp);
				newData->AddElement(tmp);
			}
			newData->SetName(inData->GetName());
			std::lock_guard<iGameAtomicMutex> DataLock(tmpLock);
			outAllDataArray->AddAttribute(CellArrays[CellArrayID].type, IG_CELL,
				newData);
		}
	};
	std::vector<std::thread> threads;
	for (int i = 0; i < threadSize; i++) {
		threads.emplace_back(std::thread(func, i));
	}
	for (int i = 0; i < threadSize; i++) { threads[i].join(); }
}

int iGameModelGeometryFilter::ExecuteWithUnstructuredGrid(
	DataObject::Pointer input, PolyData::Pointer  output) {
	return this->ExecuteWithUnstructuredGrid(input, output, nullptr);
}

int iGameModelGeometryFilter::ExecuteWithStructuredGrid(
	DataObject::Pointer input, PolyData::Pointer  output, PolyData::Pointer  exc,
	bool* extracTFace) {
	return 1;
}

int iGameModelGeometryFilter::ExecuteWithStructuredGrid(
	DataObject::Pointer input, PolyData::Pointer  output, bool* extracTFace) {
	return this->ExecuteWithStructuredGrid(input, output, nullptr, extracTFace);
}

//void ExtractDSCellGeometry(DataObject* Input, igIndex cellId,
//                           const char* cellVis) {
//    auto input = DynamicCast<VolumeMesh>(Input);
//    auto cell = input->GetVolume(cellId);
//    int cellType = cell->GetCellType();
//
//    if (cellType != IG_EMPTY_CELL) {
//        TCellArrayType& polys = localData->Polys;
//        vtkIdList* cellIds = localData->CellIds.Get();
//
//        int cellDim = cell->GetCellDimension();
//        igIndex npts = cell->PointIds->GetNumberOfIds();
//        igIndex* pts = cell->PointIds->RawPointer();
//
//        switch (cellDim) {
//                // create new points and then cell
//                //case 0:
//                //    verts.InsertNextCell(npts, pts, cellId);
//                //    break;
//
//                //case 1:
//                //    lines.InsertNextCell(npts, pts, cellId);
//                //    break;
//
//            case 2:
//                polys.InsertNextCell(npts, pts, cellId);
//                break;
//
//            case 3:
//                int numFaces = cell->GetNumberOfFaces();
//                for (auto j = 0; j < numFaces; j++) {
//                    Cell* GFace = cell->GeTFace(j);
//                    input->GetCellNeighbors(cellId, GFace->GetPointIds(),
//                                            cellIds);
//                    //如果这个面没有邻接的cell或者邻接的cell是不可见的
//                    if (cellIds->GetNumberOfIds() <= 0 ||
//                        (cellVis && !cellVis[cellIds->GetId(0)])) {
//                        polys.InsertNextCell(GFace->GetNumberOfPoints(),
//                                             GFace->PointIds->RawPointer(),
//                                             cellId);
//                    }
//                }
//                break;
//        } // switch
//    }     // non-empty cell
//} // extract dataset geometry


//struct ExtractDS : public ExtractCellBoundaries {
//    // The unstructured grid to process
//    DataObject::Pointer DataSet;
//
//    ExtractDS(DataObject* ds, const char* cellVis,
//              const unsigned char* cellGhost, const unsigned char* pointGhost)
//        : ExtractCellBoundaries(cellVis, cellGhost, pointGhost), DataSet(ds) {
//        // Point merging is always required since points are not explicitly
//        // represented and cannot be passed through to the output.
//        this->CreatePointMap(
//                DynamicCast<VolumeMesh>(DataSet)->GetNumberOfPoints());
//    }
//
//    // Initialize thread data
//    void Initialize() override { this->ExtractCellBoundaries::Initialize(); }
//
//    void Execute(igIndex cellId, igIndex endCellId) {
//        auto& localData = this->LocalData.Local();
//
//        for (; cellId < endCellId; ++cellId) {
//            //// Handle ghost cells here.  Another option was used cellVis array.
//            //if (this->CellGhosts &&
//            //    this->CellGhosts[cellId] &
//            //            MASKED_CELL_VALUE) { // Do not create surFaces in outer ghost cells.
//            //    continue;
//            //}
//
//            // If the cell is visible process it
//            if (!this->CellVis || this->CellVis[cellId]) {
//                ExtractDSCellGeometry(this->DataSet, cellId, this->CellVis,
//                                      &localData);
//            } // if cell visible
//
//        } // for all cells in this batch
//    }     // operator()
//
//    // Composite local thread data
//    void Reduce() override { this->ExtractCellBoundaries::Reduce(); }
//};


int iGameModelGeometryFilter::ExecuteWithDataSet(DataObject::Pointer Input,
	PolyData::Pointer output,
	PolyData::Pointer exc) {
	igIndex64 cellId = 0, pointId = 0;
	igIndex i;
	auto input = DynamicCast<VolumeMesh>(Input);
	igIndex64 numCells = input->GetNumberOfVolumes();
	double x[3];
	auto inPoints = input->GetPoints();
	auto inAllDataArray = input->GetAttributeSet();
	auto outAllDataArray = output->GetAttributeSet();
	CharArray::Pointer CellVisibleArray = CharArray::New();
	char* CellVisible = nullptr;
	unsigned char* cellGhosts = nullptr;
	unsigned char* pointGhosts = nullptr;


	//ArrayObject::Pointer temp;
	//if (inCD) { temp = inCD->GetArray(0); }
	//if ((!temp) || (temp->GetDataType() != IG_UNSIGNED_CHAR) ||
	//    (temp->GetNumberOfComponents() != 1)) {
	//    igDebug(this, "No appropriate ghost levels field available.");
	//} else {
	//    cellGhosts =
	//            iGameUnsignedCharArray::SafeDownCast(temp)->GetRawPointer();
	//}
	//if (inPD) { temp = inPD->GetArray(0); }
	//if ((!temp) || (temp->GetDataType() != IG_UNSIGNED_CHAR) ||
	//    (temp->GetNumberOfComponents() != 1)) {
	//    igDebug(this, "No appropriate ghost levels field available.");
	//} else {
	//    pointGhosts =
	//            iGameUnsignedCharArray::SafeDownCast(temp)->GetRawPointer();
	//}


	// Determine nature of what we have to do
	if ((!CellClipping) && (!PointClipping) && (!ExtentClipping)) {
		CellVisible = nullptr;
	}
	else {
		CellVisibleArray->Resize(numCells);
		CellVisible = CellVisibleArray->RawPointer();
	}
	// Mark cells as being visible or not
	//
	if (CellVisible) {
		CellArray::Pointer Polygons;
		igIndex vhs[256] = { 0 };
		igIndex vnum = 0;
		Point x;
		for (cellId = 0; cellId < numCells; cellId++) {
			CellVisible[cellId] = 1;
			if (CellClipping &&
				(cellId < CellMinimum || cellId > CellMaximum)) {
				CellVisible[cellId] = 0;
			}
			else {
				vnum = input->GetVolumes()->GetCellIds(cellId, vhs);
				for (i = 0; i < vnum; i++) {
					pointId = vhs[i];
					x = inPoints->GetPoint(i);
					if ((PointClipping &&
						(pointId < PointMinimum || pointId > PointMaximum)) ||
						(ExtentClipping &&
							(x[0] < Extent[0] || x[0] > Extent[1] ||
								x[1] < Extent[2] || x[1] > Extent[3] ||
								x[2] < Extent[4] || x[2] > Extent[5]))) {
						CellVisible[cellId] = 0;
						break;
					}
				}
			}
		}
	}
	CellArray::Pointer Polygons;
	//如果存在点合并的情况，会在后续处理
	output->SetPoints(inPoints);
	output->SetFaces(Polygons);

	//// The extraction process for vtkDataSet
	//ExtractDS extract(Input, CellVisible, cellGhosts, pointGhosts);
	//extract.Execute(0, numCells);
	//numCells = extract.NumCells;

	//igIndex inVcnt = input->GetNumberOfPoints();
	//igIndex outVcnt = 0;


	return 1;
}
int iGameModelGeometryFilter::ExecuteWithDataSet(DataObject::Pointer input,
	PolyData::Pointer output) {
	return this->ExecuteWithDataSet(input, output, nullptr);
}

IGAME_NAMESPACE_END