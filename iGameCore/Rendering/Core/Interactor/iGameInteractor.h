//
// Created by Sumzeek on 9/9/2024.
//

#ifndef OPENIGAME_BASEINTERACTOR_H
#define OPENIGAME_BASEINTERACTOR_H

#include "iGameCamera.h"
#include "iGameObject.h"
#include "iGameScene.h"

IGAME_NAMESPACE_BEGIN

enum MouseButton {
    NoButton = 0x00000000,
    LeftButton = 0x00000001,
    RightButton = 0x00000002,
    MiddleButton = 0x00000004
};

class Interactor : public Object {
public:
    I_OBJECT(Interactor);

    void SetScene(Scene::Pointer scene) {
        if (m_Scene != scene) {
            m_Scene = scene;
            if (m_Scene != nullptr) { this->Initialize(); }
            this->Modified();
        }
    }
    void Initialize() { m_Camera = m_Scene->m_Camera; }

    virtual void ProcessInput() {
        std::cout << "Processing input in Interactor" << std::endl;
    }

    virtual void Update() {
        std::cout << "Updating Interactor" << std::endl;
    }

    virtual void MousePressEvent(int _eventX, int _eventY,
                                 MouseButton _mouseMode) {
        std::cout << "Mouse press event at (" << _eventX
                  << ", " << _eventY
                  << ") with button " << static_cast<int>(_mouseMode)
                  << std::endl;
    }

    virtual void MouseMoveEvent(int _eventX, int _eventY) {
        std::cout << "Mouse move event at (" << _eventX
                  << ", " << _eventY
                  << ")" << std::endl;
    }

    virtual void MouseReleaseEvent(int _eventX, int _eventY) {
        std::cout << "Mouse release event at (" << _eventX
                  << ", " << _eventY
                  << ")" << std::endl;
    }

    virtual void WheelEvent(double delta) {
        std::cout << "Mouse wheel event with delta "
                  << delta << std::endl;
    }

protected:
    Interactor() {}
    ~Interactor() override = default;

    MouseButton m_MouseMode{};

    Scene::Pointer m_Scene{};
    Camera::Pointer m_Camera{};

    igm::vec2 m_OldPoint2D{};
    igm::vec2 m_NewPoint2D{};

    float m_CameraScaleSpeed{1.0f};
    float m_CameraMoveSpeed{0.01f};
};

IGAME_NAMESPACE_END

#endif //OPENIGAME_BASEINTERACTOR_H
