#ifndef OPENIGAME_SELECTION_STYLE_H
#define OPENIGAME_SELECTION_STYLE_H

#include "iGameBasicStyle.h"
#include "iGameCellArray.h"

IGAME_NAMESPACE_BEGIN
class SelectionStyle : public BasicStyle {
public:
    I_OBJECT(SelectionStyle);
    static Pointer New() { return new SelectionStyle; }

    struct Vector3Tovec3 {
        igm::vec3 operator()(const Vector3f& v) {
            return igm::vec3(v[0], v[1], v[2]);
        }
        igm::vec3 operator()(const Vector3d& v) {
            return igm::vec3(v[0], v[1], v[2]);
        }
    };

    enum SelectedType{ 
        None = -1,
        SelectPoint, 
        SelectCell
    };
    void SetSelectedType(SelectedType type);
    SelectedType GetSelectedType() const;

    void Initialize(Interactor* a, Selection* s);

    void MousePressEvent(IEvent _event) override;

    void LeftButtonMouseMove() override;
    void RightButtonMouseMove() override;
    void MiddleButtonMouseMove() override;

protected:
    SelectionStyle() = default;
    ~SelectionStyle() override = default;

    bool IsIntersectTriangle(igm::vec3 orig, igm::vec3 end, igm::vec3 v0,
                             igm::vec3 v1, igm::vec3 v2,
                             igm::vec3& intersection);

    double DistancePointToPlane(igm::vec3 point, igm::vec3 p1, igm::vec3 p2,
                                igm::vec3 p3);

    igm::vec4 GetPlane(const igm::vec3& p, const igm::vec3& normal);

    igm::vec3 GetNearWorldCoord(const igm::vec2& screenCoord,
                                const igm::mat4& invertedMvp);

    igm::vec3 GetFarWorldCoord(const igm::vec2& screenCoord,
                               const igm::mat4& invertedMvp);

    SelectedType m_Type{SelectedType::None};

    Points* m_Points{nullptr};
    CellArray* m_Cells{nullptr};
    Model* m_Model{nullptr};
    Selection* m_Selection{nullptr};
};
IGAME_NAMESPACE_END
#endif