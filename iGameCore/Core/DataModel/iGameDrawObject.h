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
	virtual bool GetClipped() { return false; }; // Gets whether this can be clipped.
	void SetExtentClipping(bool _in) { if (GetClipped()) this->m_Clip.m_Extent.m_Use = _in; }
	void SetPlaneClipping(bool _in) { if (GetClipped()) this->m_Clip.m_Plane.m_Use = _in; }
	void SetExtent(double xMin, double xMax, double yMin, double yMax,
		double zMin, double zMax, bool flip = false) {
		m_Clip.m_Extent.m_bmin[0] = xMin;
		m_Clip.m_Extent.m_bmin[1] = yMin;
		m_Clip.m_Extent.m_bmin[2] = zMin;
		m_Clip.m_Extent.m_bmax[0] = xMax;
		m_Clip.m_Extent.m_bmax[1] = yMax;
		m_Clip.m_Extent.m_bmax[2] = zMax;
		m_Clip.m_Extent.m_flip = flip;
		SetExtentClipping(true);
	}
	void SetPlane(double ox, double oy, double oz,
		double nx, double ny, double nz, bool flip = false) {
		m_Clip.m_Plane.m_origin[0] = ox;
		m_Clip.m_Plane.m_origin[1] = oy;
		m_Clip.m_Plane.m_origin[2] = oz;
		m_Clip.m_Plane.m_normal[0] = nx;
		m_Clip.m_Plane.m_normal[1] = ny;
		m_Clip.m_Plane.m_normal[2] = nz;
		m_Clip.m_Plane.m_flip = flip;
		SetPlaneClipping(true);
	}
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

	struct {
		struct {
			bool m_Use{ false };
			double m_bmin[3], m_bmax[3];
			bool m_flip{ false };
		} m_Extent;
		struct {
			bool m_Use{ false };
			double m_origin[3], m_normal[3];
			bool m_flip{ false };
		} m_Plane;
	} m_Clip; // Used for clip mesh

};

IGAME_NAMESPACE_END
#endif