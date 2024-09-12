#ifndef OPENIGAME_SQUARED_SELECTION_INTERACTOR_H
#define OPENIGAME_SQUARED_SELECTION_INTERACTOR_H

#include "iGameInteractor.h"
#include "iGamePointPicker.h"
#include "iGamePointSet.h"

IGAME_NAMESPACE_BEGIN
class SquaredSelection : public Interactor {
public:
    I_OBJECT(SquaredSelection);
    static Pointer New() { return new SquaredSelection; }


    void MousePressEvent(int posX, int posY, MouseButton mouseMode) override;
    void MouseMoveEvent(int posX, int posY) override;
    void MouseReleaseEvent(int posX, int posY) override;
    void WheelEvent(double delta) override;

    virtual void SelectElement(const std::vector<igm::vec4>& planes);

    igm::vec4 GetPlane(const igm::vec3& p, const igm::vec3& normal);

    igm::vec3 GetNearWorldCoord(const igm::vec2& screenCoord,
                                const igm::mat4& invertedMvp);

    igm::vec3 GetFarWorldCoord(const igm::vec2& screenCoord,
                               const igm::mat4& invertedMvp);

protected:
    SquaredSelection() = default;
    ~SquaredSelection() override = default;

    float width{}, height{};
    igm::mat4 mvp_invert{};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H