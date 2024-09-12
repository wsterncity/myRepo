#ifndef OPENIGAME_POINTS_SELECTION_INTERACTOR_H
#define OPENIGAME_POINTS_SELECTION_INTERACTOR_H

#include "iGameSquaredSelection.h"

IGAME_NAMESPACE_BEGIN
class PointsSelection : public SquaredSelection {
public:
    I_OBJECT(PointsSelection);
    static Pointer New() { return new PointsSelection; }

    void SetPointSet(PointSet* set, Model* model);

    void SelectElement(const std::vector<igm::vec4>& planes) override;

    bool IsPointInFrustum(const igm::vec3& p,
                          const std::vector<igm::vec4>& planes);

protected:
    PointsSelection() = default;
    ~PointsSelection() override = default;

    PointSet* m_Points{nullptr};
    Model* m_Model{nullptr};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H