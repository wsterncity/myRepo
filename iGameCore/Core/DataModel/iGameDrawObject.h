#ifndef iGameDrawObject_h
#define iGameDrawObject_h

#include "iGameDataObject.h"
#include "iGameMarker.h"
#include "iGamePoints.h"
#include "iGameIdArray.h"

IGAME_NAMESPACE_BEGIN
class DrawObject : public DataObject {
public:
	I_OBJECT(DrawObject);
	static Pointer New() { return new DrawObject; }
	IGenum GetDataObjectType() const { return IG_DRAW_OBJECT; }
	IGsize GetRealMemorySize() override { return 0; };
protected:
	DrawObject() {};
	~DrawObject() override = default;

protected:
	GLVertexArray m_PointVAO, m_VertexVAO,  m_LineVAO, m_TriangleVAO;
	GLBuffer m_PositionVBO, m_ColorVBO, m_NormalVBO, m_TextureVBO;
	GLBuffer m_PointEBO, m_VertexEBO,m_LineEBO, m_TriangleEBO;
	GLVertexArray m_CellVAO;
	GLBuffer m_CellPositionVBO, m_CellColorVBO;

	FloatArray::Pointer m_Positions{};
	FloatArray::Pointer m_Colors{};
	IdArray::Pointer m_PointIndices{};
	IdArray::Pointer m_LineIndices{};
	IdArray::Pointer m_TriangleIndices{};
	UnsignedIntArray::Pointer M_VertexIndices{};
	UnsignedIntArray::Pointer M_LineIndices{};
	UnsignedIntArray::Pointer M_TriangleIndices{};
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