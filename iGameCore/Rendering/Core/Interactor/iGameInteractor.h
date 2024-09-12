//
// Created by Sumzeek on 9/9/2024.
//

#ifndef OPENIGAME_BASEINTERACTOR_H
#define OPENIGAME_BASEINTERACTOR_H

#include "iGameCamera.h"
#include "iGameObject.h"
#include "iGameScene.h"
#include <format>

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
        std::cout << "Processing input in Interactor." << std::endl;
    }

    virtual void Update() { std::cout << "Updating Interactor." << std::endl; }


    virtual void MousePressEvent(int eventX, int eventY,
                                 MouseButton _mouseMode) {
        std::cout << std::format(
                "Mouse press event at ({}, {}) with button {}\n", eventX,
                eventY, static_cast<int>(_mouseMode));
    }

    virtual void MouseMoveEvent(int eventX, int eventY) {
        std::cout << std::format("Mouse move event at ({}, {})\n", eventX,
                                 eventY);
    }

    virtual void MouseReleaseEvent(int eventX, int eventY) {
        std::cout << std::format("Mouse release event at ({}, {})\n", eventX,
                                 eventY);
    }

    virtual void WheelEvent(double delta) {
        std::cout << std::format("Mouse wheel event with delta {}\n", delta);
    }


protected:
    Interactor() = default;
    ~Interactor() override = default;

    MouseButton m_MouseMode{};

    Scene::Pointer m_Scene{};
    Camera::Pointer m_Camera{};

    igm::vec2 m_OldPoint2D{};
    igm::vec2 m_NewPoint2D{};
};

IGAME_NAMESPACE_END

#endif //OPENIGAME_BASEINTERACTOR_H
