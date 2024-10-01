#include "iGameDrawObject.h"

IGAME_NAMESPACE_BEGIN

void DrawObject::ConvertToDrawableData() {
    throw std::runtime_error(
            "The function ConvertToDrawableData() has not been implemented");
}

IGenum DrawObject::GetDataObjectType() const { return IG_DRAW_OBJECT; }

IGsize DrawObject::GetRealMemorySize() { return 0; };

void DrawObject::SetVisibility(bool f) { this->m_Visibility = f; }

bool DrawObject::GetVisibility() { return m_Visibility; }

void DrawObject::SetViewStyle(IGenum mode) {
    /*
     * e.g. mode = IG_WIREFRAME | IG_SURFACE, means that the model shows the wireframe and surface.
     * */
    m_ViewStyle = mode;
}

void DrawObject::AddViewStyle(IGenum mode) { m_ViewStyle |= mode; }

void DrawObject::RemoveViewStyle(IGenum mode) { m_ViewStyle &= ~mode; }

unsigned int DrawObject::GetViewStyle() { return m_ViewStyle; }

bool DrawObject::GetClipped() {
    return false;
}; // Gets whether this can be clipped.

void DrawObject::SetExtentClipping(bool _in) {
    if (GetClipped()) this->m_Clip.m_Extent.m_Use = _in;
}

void DrawObject::SetPlaneClipping(bool _in) {
    if (GetClipped()) this->m_Clip.m_Plane.m_Use = _in;
}

void DrawObject::SetExtent(double xMin, double xMax, double yMin, double yMax,
                           double zMin, double zMax, bool flip) {
    m_Clip.m_Extent.m_bmin[0] = xMin;
    m_Clip.m_Extent.m_bmin[1] = yMin;
    m_Clip.m_Extent.m_bmin[2] = zMin;
    m_Clip.m_Extent.m_bmax[0] = xMax;
    m_Clip.m_Extent.m_bmax[1] = yMax;
    m_Clip.m_Extent.m_bmax[2] = zMax;
    m_Clip.m_Extent.m_flip = flip;
    SetExtentClipping(true);
}

void DrawObject::SetPlane(double ox, double oy, double oz, double nx, double ny,
                          double nz, bool flip) {
    m_Clip.m_Plane.m_origin[0] = ox;
    m_Clip.m_Plane.m_origin[1] = oy;
    m_Clip.m_Plane.m_origin[2] = oz;
    m_Clip.m_Plane.m_normal[0] = nx;
    m_Clip.m_Plane.m_normal[1] = ny;
    m_Clip.m_Plane.m_normal[2] = nz;
    m_Clip.m_Plane.m_flip = flip;
    SetPlaneClipping(true);
}

IGAME_NAMESPACE_END