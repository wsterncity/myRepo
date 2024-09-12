#ifndef OPENIGAME_POINT_PICKED_INTERACTOR_H
#define OPENIGAME_POINT_PICKED_INTERACTOR_H

#include "iGameInteractor.h"
#include "iGamePointPicker.h"
#include "iGamePointSet.h"

IGAME_NAMESPACE_BEGIN
class PointPickedInteractor : public Interactor {
public:
    I_OBJECT(PointPickedInteractor);
    static Pointer New() { return new PointPickedInteractor; }

    void MousePressEvent(int posX, int posY, MouseButton mouseMode) override;
    void MouseMoveEvent(int eventX, int eventY) override;
    void MouseReleaseEvent(int eventX, int eventY) override;
    void WheelEvent(double delta) override;

    void SetPointSet(PointSet* set, Model* model);

protected:
    PointPickedInteractor() = default;
    ~PointPickedInteractor() override = default;

    PointSet* m_Points{nullptr};
    Model* m_Model{nullptr};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H