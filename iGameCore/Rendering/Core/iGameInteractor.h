#ifndef OPENIGAME_INTERACTOR_H
#define OPENIGAME_INTERACTOR_H

#include "iGameCamera.h"
#include "iGameObject.h"
#include "iGameScene.h"
#include "igm/igm.h"
#include "igm/transform.h"

IGAME_NAMESPACE_BEGIN

enum MouseButton {
    NoButton = 0x00000000,
    LeftButton = 0x00000001,
    RightButton = 0x00000002,
    MiddleButton = 0x00000004
};

enum EditMode { MODELVIEW = 0x00000000, PICKITEM = 0x00000001 };

class Interactor : public Object {
public:
    I_OBJECT(Interactor);
    static Pointer New() { return new Interactor; }

    void SetScene(Scene::Pointer scene) {
        if (m_Scene != scene) {
            m_Scene = scene;
            if (m_Scene != nullptr) { this->Initialize(); }
            this->Modified();
        }
    }

    void MousePressEvent(int _eventX, int _eventY, MouseButton _mouseMode);
    void MouseMoveEvent(int _eventX, int _eventY);
    void MouseReleaseEvent(int _eventX, int _eventY);
    void WheelEvent(double delta);

private:
    void Initialize();
    void ModelRotation();
    void ViewTranslation();
    void MapToSphere(igm::vec3& old_v3D, igm::vec3& new_v3D);
    void UpdateCameraMoveSpeed(const igm::vec4& center);

protected:
    Interactor();
    ~Interactor() override;

    MouseButton m_MouseMode{};
    EditMode m_EditMode{};

    Camera::Pointer m_Camera{};
    Scene::Pointer m_Scene{};

    igm::vec2 m_OldPoint2D{};
    igm::vec2 m_NewPoint2D{};

    float m_CameraScaleSpeed{0.1f};
    float m_CameraMoveSpeed{0.01f};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H