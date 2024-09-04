#ifndef StructuredMesh_h
#define StructuredMesh_h
#include "iGamePoints.h"
#include "iGameQuad.h"
#include "iGameHexahedron.h"
#include "iGameCellArray.h"
#include "iGameAttributeSet.h"
#include "iGamePointSet.h"
IGAME_NAMESPACE_BEGIN
class StructuredMesh : public PointSet {
public:
	I_OBJECT(StructuredMesh);
	static StructuredMesh* New() { return new StructuredMesh; }
	~StructuredMesh() override;

	Points::Pointer GetPoints() { return this->m_Points; }
	AttributeSet::Pointer GetPointData() { return this->m_Attributes; }
	CellArray::Pointer GetFaces() { return this->Quads; }
	CellArray::Pointer GetQuads() { return this->Quads; }
	CellArray::Pointer GetCells() { return this->Hexahedrons; }
	CellArray::Pointer GetHexahedrons() { return this->Hexahedrons; }

	void SetPoints(Points::Pointer o) { this->m_Points = o; }
	void SetPointData(AttributeSet::Pointer o) { this->m_Attributes = o; }

	unsigned int GetNumberOfPoints() { return this->m_Points == nullptr ? 0 : this->m_Points->GetNumberOfPoints(); }
	unsigned int GetNumberOfQuads() { return this->Quads == nullptr ? 0 : this->Quads->GetNumberOfCells(); }
	unsigned int GetNumberOfFaces() { return this->Quads == nullptr ? 0 : this->Quads->GetNumberOfCells(); }
	unsigned int GetNumberOfHexahedrons() { return this->Hexahedrons == nullptr ? 0 : this->Hexahedrons->GetNumberOfCells(); }
	unsigned int GetNumberOfCells() { return this->Hexahedrons == nullptr ? 0 : this->Hexahedrons->GetNumberOfCells(); }



	void SetDimensionSize(igIndex s[3]) {
		std::copy(s, s + 3, this->size);
		if (size[2] <= 1) {
			size[2] = 1;
			this->Dimension = 2;
		}
		else this->Dimension = 3;
	}
	void SetExtent(igIndex e[6]) { std::copy(e, e + 6, this->size); }
	igIndex* GetDimensionSize() { return this->size; }
	igIndex* GetExtent() { return this->extent; }
	igIndex GetDimension() { return this->Dimension; }

	void AddPoint(Point& p) {
		if (!this->m_Points)this->m_Points = Points::New();
		this->m_Points->AddPoint(p);
	}
	void GenStructuredCellConnectivities() {
		if (size[2] <= 1) {
			size[2] = 1;
			this->Dimension = 2;
		}
		igIndex i = 0, j = 0, k = 0;
		igIndex vhs[8] = { 0 };
		igIndex st = 0;
		this->Quads = CellArray::New();
		if (this->Dimension == 3) {
			this->Hexahedrons = CellArray::New();
			this->Hexahedrons->Resize((size[0] - 1) * (size[1] - 1) * (size[2] - 1));
			igIndex tmpvhs[8] = {
				0,1,1 + size[0] * size[1],size[0] * size[1],
				size[0],1 + size[0],1 + size[0] + size[0] * size[1],size[0] + size[0] * size[1]
			};
			std::cout << size[0] << ' ' << size[1] << ' ' << size[2] << '\n';
			for (k = 0; k < size[2] - 1; ++k) {
				for (j = 0; j < size[1] - 1; ++j) {
					st = j * size[0] + k * size[0] * size[1];
					for (i = 0; i < size[0] - 1; ++i) {
						for (int it = 0; it < 8; it++) {
							vhs[it] = st + tmpvhs[it];
						}
						Hexahedrons->AddCellIds(vhs, 8);
						st++;
					}
				}
			}
			this->Quads->Resize(size[2] * (size[1] - 1) * (size[0] - 1) +
				size[0] * (size[1] - 1) * (size[2] - 1) +
				size[1] * (size[2] - 1) * (size[0] - 1));
			// ij面的定义
			tmpvhs[1] = 1;
			tmpvhs[2] = 1 + size[0];
			tmpvhs[3] = size[0];
			for (k = 0; k < size[2]; ++k) {
				for (j = 0; j < size[1] - 1; ++j) {
					st = j * size[0] + k * size[0] * size[1];
					for (i = 0; i < size[0] - 1; ++i) {
						for (int it = 0; it < 4; it++) {
							vhs[it] = st + tmpvhs[it];
						}
						st++;
						Quads->AddCellIds(vhs, 4);
					}
				}
			}
			// ik方向面的定义
			tmpvhs[1] = 1;
			tmpvhs[2] = 1 + size[0] * size[1];
			tmpvhs[3] = size[0] * size[1];
			for (j = 0; j < size[1]; j++) {
				for (k = 0; k < size[2] - 1; k++) {
					st = j * size[0] + k * size[0] * size[1];
					for (i = 0; i < size[0] - 1; i++) {
						for (int it = 0; it < 4; it++) {
							vhs[it] = st + tmpvhs[it];
						}
						st++;
						Quads->AddCellIds(vhs, 4);
					}
				}
			}

			// jk方向面的定义
			tmpvhs[1] = size[0];
			tmpvhs[2] = size[0] + size[0] * size[1];
			tmpvhs[3] = size[0] * size[1];
			for (i = 0; i < size[0]; i++) {
				for (k = 0; k < size[2] - 1; k++) {
					st = i + k * size[0] * size[1];
					for (j = 0; j < size[1] - 1; j++) {
						for (int it = 0; it < 4; it++) {
							vhs[it] = st + tmpvhs[it];
						}
						st += size[0];
						Quads->AddCellIds(vhs, 4);
					}
				}
			}
		}
		else {
			this->Quads->Resize((size[0] - 1) * (size[1] - 1));
			igIndex tmpvhs[4] = { 0,1,size[0] + 1,size[0] };
			for (j = 0; j < size[1] - 1; ++j) {
				st = j * size[0];
				for (i = 0; i < size[0] - 1; ++i) {
					for (int it = 0; it < 4; it++) {
						vhs[it] = st + tmpvhs[it];
					}
					st++;
					Quads->AddCellIds(vhs, 4);
				}
			}
		}
	}
	igIndex GetPointIndex(igIndex i, igIndex j, igIndex k) {
		return i + j * size[0] + k * size[0] * size[1];
	}

protected:
	StructuredMesh() {};

	Points::Pointer m_Points{ nullptr };
	AttributeSet::Pointer m_Attributes{};
	CellArray::Pointer Quads{ nullptr };
	CellArray::Pointer Hexahedrons{ nullptr };

	igIndex Dimension = 3;
	// ni,nj,nk
	igIndex size[3] = { 0,0,0 };
	//ist,ied,jst,jed,kst,ked
	igIndex extent[6] = { 0,0,0,0,0,0 };

public:
	int GetDataObjectType() {
		return IG_STRUCTURED_MESH;
	}
public:
	void Draw(Scene*) override;
	void ConvertToDrawableData() override;
	bool IsDrawable() override { return true; }


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

inline StructuredMesh::~StructuredMesh()
{
}


IGAME_NAMESPACE_END
#endif