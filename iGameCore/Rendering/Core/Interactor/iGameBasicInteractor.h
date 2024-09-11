#ifndef OPENIGAME_INTERACTOR_H
#define OPENIGAME_INTERACTOR_H

#include "iGameInteractor.h"
#include "igm/igm.h"
#include "igm/transform.h"
#include <Eigen/Dense>

IGAME_NAMESPACE_BEGIN

class BasicInteractor : public Interactor {
public:
    I_OBJECT(BasicInteractor);
    static Pointer New() { return new BasicInteractor; }

    void MousePressEvent(int eventX, int eventY,
                         MouseButton mouseMode) override;
    void MouseMoveEvent(int eventX, int eventY) override;
    void MouseReleaseEvent(int eventX, int eventY) override;
    void WheelEvent(double delta) override;

private:
    void ModelRotation();
    void ViewTranslation();
    void MapToSphere(igm::vec3& old_v3D, igm::vec3& new_v3D);
    void UpdateCameraMoveSpeed(const igm::vec4& center);

protected:
    BasicInteractor() = default;
    ~BasicInteractor() override = default;

    float m_CameraScaleSpeed{1.0f};
    float m_CameraMoveSpeed{0.01f};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H