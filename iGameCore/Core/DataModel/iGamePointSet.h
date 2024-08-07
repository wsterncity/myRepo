#ifndef iGamePointSet_h
#define iGamePointSet_h

#include "iGameDataObject.h"
#include "iGameMarker.h"
#include "iGamePoints.h"
#include "iGamePropertySet.h"

IGAME_NAMESPACE_BEGIN
class PointSet : public DataObject {
public:
    I_OBJECT(PointSet);
    static Pointer New() { return new PointSet; }

    // Set/Get point array
    void SetPoints(Points::Pointer points);
    Points::Pointer GetPoints();

    // Get the number of points
    IGsize GetNumberOfPoints();

    // Get/Set a point by index ptId
    const Point& GetPoint(const IGsize ptId) const;
    void SetPoint(const IGsize ptId, const Point& p);

    // Add a point to back
    virtual IGsize AddPoint(const Point& p);

    // Request data edit state, only in this state,
    // can perform the adding and delete operation.
    // Adding operations also can be done via GetPoints().
    virtual void RequestEditStatus();

    // Detele point by index ptId
    virtual void DeletePoint(const IGsize ptId);

    // Whether point is deleted or not
    bool IsPointDeleted(const IGsize ptId);

    // Garbage collection to free memory that has been removed
    virtual void GarbageCollection();

    // Whether is in data edit state or not
    bool InEditStatus();

    // Enable data edit state
    void MakeEditStatusOn();
    // Unable data edit state
    void MakeEditStatusOff();

protected:
    PointSet();
    ~PointSet() override = default;

    // Request point data edit state
    void RequestPointStatus();

    // Compute model bounding box
    void ComputeBoundingBox() override;

    Points::Pointer m_Points{};                  // The point array
    DeleteMarker::Pointer m_PointDeleteMarker{}; // The marker for delete point
    bool m_InEditStatus{false};

public:
    void Draw(Scene*) override;
    void DrawPhase1(Scene*) override;
    void DrawPhase2(Scene*) override;
    void DrawPhase3(Scene*) override;
    void ConvertToDrawableData() override;
    bool IsDrawable() override { return true; }
    void ViewCloudPicture(int index, int demension = -1) override;

    virtual void SetAttributeWithPointData(ArrayObject::Pointer attr,
                                           igIndex i = -1);

private:
    GLVertexArray m_PointVAO;
    GLBuffer m_PositionVBO, m_ColorVBO;
    GLBuffer m_PointEBO;

    FloatArray::Pointer m_Positions{};
    FloatArray::Pointer m_Colors{};
    bool m_UseColor{false};
    int m_PointSize{40};

    ArrayObject::Pointer m_ViewAttribute;
    int m_ViewDemension;
};


IGAME_NAMESPACE_END
#endif