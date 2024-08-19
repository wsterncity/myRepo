#ifndef OPENIGAME_POINTS_SELECTION_INTERACTOR_H
#define OPENIGAME_POINTS_SELECTION_INTERACTOR_H

#include "iGameSquaredSelection.h"

IGAME_NAMESPACE_BEGIN
class PointsSelection : public SquaredSelection {
public:
    I_OBJECT(PointsSelection);
    static Pointer New() { return new PointsSelection; }

    void SetPointSet(PointSet* set, Model* model) {
        m_Points = set;
        m_Model = model;
    }

    void SelectElement(const std::vector<igm::vec4>& planes) override
    {
        m_Model->GetPointPainter()->Clear();
        for (int i = 0; i < m_Points->GetNumberOfPoints(); i++) {
            auto& p = m_Points->GetPoint(i);

            if (IsPointInFrustum(igm::vec3(p[0], p[1], p[2]), planes)) {
                m_Model->GetPointPainter()->DrawPoint(p);
            }
        }
    }

    bool IsPointInFrustum(const igm::vec3& p,
        const std::vector<igm::vec4>& planes) {

        for (int i = 0; i < 6; i++) {
            auto& plane = planes[i];
            float dist = p.x * plane.x + p.y * plane.y + p.z * plane.z + plane.w;
            if (dist < 0) {
                return false;
            }
        }

        return true;
    }

protected:
    PointsSelection() {}
    ~PointsSelection() override = default;

    PointSet* m_Points{nullptr};
    Model* m_Model{nullptr};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H