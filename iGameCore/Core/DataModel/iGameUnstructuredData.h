#ifndef iGameUnstructuredData_h
#define iGameUnstructuredData_h

#include "iGamePointSet.h"
#include "iGameScene.h"

#include "iGameEmptyCell.h"
#include "iGameLine.h"
#include "iGamePolyLine.h"
#include "iGameTriangle.h"
#include "iGameQuad.h"
#include "iGamePolygon.h"
#include "iGameTetra.h"
#include "iGameHexahedron.h"
#include "iGamePrism.h"
#include "iGamePyramid.h"

IGAME_NAMESPACE_BEGIN
class UnstructuredData : public PointSet {
public:
    I_OBJECT(UnstructuredData);
    static Pointer New() { return new UnstructuredData; }

    Cell* GetCell(const IGsize cellId) {

        Cell* cell = GetTypedCell(cellId);
        
    }

    IGenum GetCellType(const IGsize cellId) const { 
        return m_Types->GetValue(cellId);
    }

protected:
    UnstructuredData() {}
    ~UnstructuredData() override = default;

    Cell* GetTypedCell(const IGsize cellId) {
        Cell* cell = nullptr;
        switch (GetCellType(cellId)) {
            case IG_LINE: {
                if (m_Line == nullptr) { m_Line = Line::New(); }
                cell = m_Line.get();
            } break;
            case IG_POLY_LINE: {
                if (m_PolyLine == nullptr) { m_PolyLine = PolyLine::New(); }
                cell = m_PolyLine.get();
            } break;
            case IG_TRIANGLE: {
                if (m_Triangle == nullptr) { m_Triangle = Triangle::New(); }
                cell = m_Triangle.get();
            } break;
            case IG_QUAD: {
                if (m_Quad == nullptr) { m_Quad = Quad::New(); }
                cell = m_Quad.get();
            } break;
            case IG_POLYGON: {
                if (m_Polygon == nullptr) { m_Polygon = Polygon::New(); }
                cell = m_Polygon.get();
            } break;
            case IG_TETRA: {
                if (m_Tetra == nullptr) { m_Tetra = Tetra::New(); }
                cell = m_Tetra.get();
            } break;
            case IG_HEXAHEDRON: {
                if (m_Hexahedron == nullptr) { m_Hexahedron = Hexahedron::New(); }
                cell = m_Hexahedron.get();
            } break;
            case IG_PRISM: {
                if (m_Prism == nullptr) { m_Prism = Prism::New(); }
                cell = m_Prism.get();
            } break;
            case IG_PYRAMID: {
                if (m_Pyramid == nullptr) { m_Pyramid = Pyramid::New(); }
                cell = m_Pyramid.get();
            } break;
            default: {
                if (m_EmptyCell == nullptr) { m_EmptyCell = EmptyCell::New(); }
                cell = m_EmptyCell.get();
            } break;
        }
        return cell;
    }

    CellArray::Pointer m_Cells;
    UnsignedIntArray::Pointer m_Types;

private:
    EmptyCell::Pointer m_EmptyCell{};
    Line::Pointer m_Line{};
    PolyLine::Pointer m_PolyLine{};
    Triangle::Pointer m_Triangle{};
    Quad::Pointer m_Quad{};
    Polygon::Pointer m_Polygon{};
    Tetra::Pointer m_Tetra{};
    Hexahedron::Pointer m_Hexahedron{};
    Prism::Pointer m_Prism{};
    Pyramid::Pointer m_Pyramid{};

public:
	void Draw(Scene*) override {}
	void ConvertToDrawableData() override{}
	bool IsDrawable() override { return true; }
};
IGAME_NAMESPACE_END
#endif