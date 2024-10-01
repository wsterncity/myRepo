#ifndef iGameDrawObject_h
#define iGameDrawObject_h

#include "iGameIdArray.h"
#include "iGameMarker.h"
#include "iGameMeshlet.h"
#include "iGamePoints.h"

#include "OpenGL/GLBuffer.h"
#include "OpenGL/GLShader.h"
#include "OpenGL/GLTexture2d.h"
#include "OpenGL/GLVertexArray.h"

#include "iGameMeshlet.h"

IGAME_NAMESPACE_BEGIN
class Model;

class DrawObject : public Object {
public:
    I_OBJECT(DrawObject);
    static Pointer New() { return new DrawObject; }

protected:
    DrawObject(){};
    ~DrawObject() override = default;

public:
    void SetVisibility(bool f);
    bool GetVisibility();
    /*ViewStyle's detail. See iGameType.h */
    void SetViewStyle(IGenum mode);
    void AddViewStyle(IGenum mode);
    void RemoveViewStyle(IGenum mode);
    unsigned int GetViewStyle();

    virtual bool GetClipped();
    void SetExtentClipping(bool _in);
    void SetPlaneClipping(bool _in);
    void SetExtent(double xMin, double xMax, double yMin, double yMax,
                   double zMin, double zMax, bool flip = false);
    void SetPlane(double ox, double oy, double oz, double nx, double ny,
                  double nz, bool flip = false);

    virtual IGenum GetDataObjectType() const;
    virtual IGsize GetRealMemorySize();

protected:
    void Create();
    virtual void ConvertToDrawableData();

protected:
    unsigned int m_ViewStyle{0};
    bool m_Visibility{true};
    bool m_Drawable{false};

protected:
    GLVertexArray m_PointVAO, m_VertexVAO, m_LineVAO, m_TriangleVAO;
    GLBuffer m_PositionVBO, m_ColorVBO, m_NormalVBO, m_TextureVBO;
    GLBuffer m_PointEBO, m_VertexEBO, m_LineEBO, m_TriangleEBO;
    GLVertexArray m_CellVAO;
    GLBuffer m_CellPositionVBO, m_CellColorVBO;

    FloatArray::Pointer m_Positions{};
    FloatArray::Pointer m_Colors{};
    IdArray::Pointer m_PointIndices{};
    IdArray::Pointer m_LineIndices{};
    IdArray::Pointer m_TriangleIndices{};
    //UnsignedIntArray::Pointer M_VertexIndices{};
    //UnsignedIntArray::Pointer M_LineIndices{};
    //UnsignedIntArray::Pointer M_TriangleIndices{};

    bool m_Flag{false};
    bool m_UseColor{false};
    bool m_ColorWithCell{false};
    int m_PointSize{8};
    int m_LineWidth{1};
    int m_CellPositionSize{};
    float m_Transparency{1.0f};

    ArrayObject::Pointer m_ViewAttribute{};
    int m_ViewDemension{};

    struct {
        struct {
            bool m_Use{false};
            double m_bmin[3], m_bmax[3];
            bool m_flip{false};
        } m_Extent;
        struct {
            bool m_Use{false};
            double m_origin[3], m_normal[3];
            bool m_flip{false};
        } m_Plane;
    } m_Clip; // Used for clip mesh

    friend class Model;

#ifdef IGAME_OPENGL_VERSION_460
    Meshlet::Pointer m_Meshlets{Meshlet::New()};
#endif
};

IGAME_NAMESPACE_END
#endif