#ifndef OPENIGAME_BASIC_STYLE_H
#define OPENIGAME_BASIC_STYLE_H

#include "iGameInteractorStyle.h"
#include "iGameScene.h"
#include "iGameCamera.h"

IGAME_NAMESPACE_BEGIN
class BasicStyle : public InteractorStyle {
public:
    I_OBJECT(BasicStyle);
    static Pointer New() { return new BasicStyle; }

    void Initialize(Interactor* a) override;
    void MousePressEvent(IEvent _event) override;
    void MouseMoveEvent(IEvent _event) override;
    void MouseReleaseEvent(IEvent _event) override;
    void WheelEvent(IEvent _event) override;

    virtual void LeftButtonMouseMove();
    virtual void RightButtonMouseMove();
    virtual void MiddleButtonMouseMove();

protected:
    BasicStyle() = default;
    ~BasicStyle() override = default;

    void ModelRotation();
    void ViewTranslation();
    void MapToSphere(igm::vec3& old_v3D, igm::vec3& new_v3D);
    void UpdateCameraMoveSpeed(const igm::vec4& center);

    Interactor* m_Interactor{nullptr};
    Scene* m_Scene{nullptr};
    Camera* m_Camera{nullptr};

    igm::vec2 m_OldPoint2D{};
    igm::vec2 m_NewPoint2D{};
    float m_CameraScaleSpeed{1.0f};
    float m_CameraMoveSpeed{0.01f};
    MouseButton m_MouseMode{NoButton};
};
IGAME_NAMESPACE_END
#endif