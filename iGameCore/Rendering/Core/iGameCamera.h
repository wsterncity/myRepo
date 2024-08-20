/**
* @file
* @brief    Camera operations for rendering scenes.
* @details  iGameRendering module.
* @version  v1.0.0
* @date     4/13/2024
* @author   Sumzeek
* @par Copyright(c): Hangzhou Dianzi University iGame Laboratory
* @par History:
*    v1.0.0: Sumzeek, 4/13/2024, first create this file\n
*/

#ifndef OPENIGAME_CAMERA_H
#define OPENIGAME_CAMERA_H


#include "iGameObject.h"
#include "iGameRenderingMacro.h"
#include "igm/igm.h"
#include "igm/transform.h"

IGAME_NAMESPACE_BEGIN

class Viewport : public Object {
public:
    I_OBJECT(Viewport)
    static Pointer New() { return new Viewport; }

public:
    void SetViewPort(uint32_t width, uint32_t height) {
        m_Size.x = width;
        m_Size.y = height;
    };
    void SetDevicePixelRatio(int devicePixelRatio) {
        m_DevicePixelRatio = devicePixelRatio;
    };

    igm::uvec2 GetViewPort() { return m_Size * m_DevicePixelRatio; };
    int GetDevicePixelRatio() { return m_DevicePixelRatio; };

    template<typename FloatT>
    FloatT aspect() const {
        return FloatT(m_Size.x) / FloatT(m_Size.y);
    }

private:
    igm::uvec2 m_Offset = igm::uvec2{0, 0};
    igm::uvec2 m_Size = igm::uvec2{800, 600};
    int m_DevicePixelRatio = 1;

protected:
    Viewport() = default;
    ~Viewport() override = default;
};

class Viewer : public Viewport {
public:
    I_OBJECT(Viewer)
    static Pointer New() { return new Viewer; }

public:
    void SetNearPlane(float near) { nearPlane = near; };
    //void SetFarPlane(float far) { farPlane = far; };
    float GetNearPlane() { return nearPlane; };
    //float GetFarPlane() { return farPlane; };

    igm::mat4 GetProjectionMatrix() {
        return igm::perspectiveRH_ZO(static_cast<float>(igm::radians(fov)),
                                     aspect<float>(), nearPlane);
    };

    igm::mat4 GetProjectionMatrixReversedZ() {
        return igm::perspectiveRH_OZ(static_cast<float>(igm::radians(fov)),
                                     aspect<float>(), nearPlane);
    }

protected:
    float fov = 45.0f;
    float nearPlane = 1e-5f;
    //float farPlane = 100.0f;

protected:
    Viewer() = default;
    ~Viewer() override = default;
};

class Camera : public Viewer {
public:
    I_OBJECT(Camera)
    static Pointer New() { return new Camera; }

private:
    enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;

public:
    void Initialize(igm::vec3 position = igm::vec3(0.0f, 0.0f, 0.0f),
                    igm::vec3 up = igm::vec3(0.0f, 1.0f, 0.0f)) {
        m_Position = position;
        m_WorldUp = up;
        m_Yaw = YAW;
        m_Pitch = PITCH;
        updateCameraVectors();
    }

    void Initialize(float posX, float posY, float posZ, float upX, float upY,
                    float upZ, float yaw, float pitch) {
        m_Position = igm::vec3(posX, posY, posZ);
        m_WorldUp = igm::vec3(upX, upY, upZ);
        m_Yaw = yaw;
        m_Pitch = pitch;
        updateCameraVectors();
    }

    void SetCameraPos(igm::vec3 pos) { m_Position = pos; }

    void SetCameraPos(float x, float y, float z) { m_Position = {x, y, z}; }

    igm::vec3 GetCameraPos() const { return m_Position; }

    igm::vec3 GetCameraUp() const { return m_Up; }

    igm::mat4 GetViewMatrix() {
        return igm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = m_MovementSpeed * deltaTime;
        float origin_y = m_Position.y;
        if (direction == FORWARD) {
            m_Position += m_Front * velocity;
            m_Position.y = origin_y;
        }
        if (direction == BACKWARD) {
            m_Position -= m_Front * velocity;
            m_Position.y = origin_y;
        }
        if (direction == LEFT) m_Position -= m_Right * velocity;
        if (direction == RIGHT) m_Position += m_Right * velocity;
        if (direction == UP) m_Position.y += velocity;
        if (direction == DOWN) m_Position.y -= velocity;
    }

    void ProcessMouseMovement(float xoffset, float yoffset,
                              bool constrainPitch = true) {
        xoffset *= m_MouseSensitivity;
        yoffset *= m_MouseSensitivity;

        m_Yaw += xoffset;
        m_Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch) {
            if (m_Pitch > 89.0f) m_Pitch = 89.0f;
            if (m_Pitch < -89.0f) m_Pitch = -89.0f;
        }

        // Update m_Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset) {
        m_Zoom -= (float) yoffset;
        if (m_Zoom < 1.0f) m_Zoom = 1.0f;
        if (m_Zoom > 45.0f) m_Zoom = 45.0f;
    }

    void moveXY(float offsetX, float offsetY) {
        m_Position += igm::vec3{offsetX, offsetY, 0.0f};
    }

    void moveZ(float offsetZ) { m_Position.z += offsetZ; }

    //void SetViewMatrix(igm::mat4& _viewMatrix) {
    //    this->m_ViewMatrix = _viewMatrix;
    //    igm::mat4 inv_view = _viewMatrix.invert();
    //    m_Position = igm::vec3(inv_view[0][3], inv_view[1][3], inv_view[2][3]);
    //    updateCameraVectors();
    //}

private:
    Camera()
        : m_Front(igm::vec3(0.0f, 0.0f, -1.0f)), m_MovementSpeed(SPEED),
          m_MouseSensitivity(SENSITIVITY), m_Zoom(ZOOM) {}
    ~Camera() override = default;

    void updateCameraVectors() {
        // Calculate the new m_Front vector
        igm::vec3 front;
        front.x = cos(igm::radians(m_Yaw)) * cos(igm::radians(m_Pitch));
        front.y = sin(igm::radians(m_Pitch));
        front.z = sin(igm::radians(m_Yaw)) * cos(igm::radians(m_Pitch));
        m_Front = front.normalize();
        // Also re-calculate the Right and Up vector
        m_Right = (igm::cross(m_Front, m_WorldUp)).normalized();
        m_Up = (igm::cross(m_Right, m_Front)).normalized();
    }

    // Camera attributes
    igm::vec3 m_Position;
    igm::vec3 m_Front;
    igm::vec3 m_Up;
    igm::vec3 m_Right;
    igm::vec3 m_WorldUp;

    // Euler angles
    float m_Yaw;
    float m_Pitch;

    // Camera options
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Zoom;
};
IGAME_NAMESPACE_END

#endif // OPENIGAME_CAMERA_H
