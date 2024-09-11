#ifndef OPENIGAME_INTERACTOR_H
#define OPENIGAME_INTERACTOR_H

#include <Eigen/Dense>
#include "iGameInteractor.h"
#include "igm/igm.h"
#include "igm/transform.h"

IGAME_NAMESPACE_BEGIN

class BasicInteractor : public Interactor {
public:
    I_OBJECT(BasicInteractor);
    static Pointer New() { return new BasicInteractor; }

    virtual void MousePressEvent(int _eventX, int _eventY,
                                 MouseButton _mouseMode) {
        m_OldPoint2D = {float(_eventX), (float) _eventY};
        m_MouseMode = _mouseMode;
    };

    virtual void MouseMoveEvent(int _eventX, int _eventY) {
        m_NewPoint2D = {(float) _eventX, (float) _eventY};
        if (m_OldPoint2D == m_NewPoint2D) { return; }

        switch (m_MouseMode) {
            case NoButton:
                break;
            case LeftButton:
                ModelRotation();
                break;
            case RightButton:
                ViewTranslation();
                break;
            case MiddleButton:
                break;
            default:
                break;
        }
        m_OldPoint2D = m_NewPoint2D;
    };

    virtual void MouseReleaseEvent(int _eventX, int _eventY) {
        m_MouseMode = NoButton;
    };

    virtual void WheelEvent(double delta) {
        float wheelMoveDirection = 0.0f;
        if (delta == 0) {
            std::cout << "The wheel movement given to the interactor is 0"
                      << std::endl;
            return;
        } else if (delta > 0.0) {
            wheelMoveDirection = 1.0f;
        } else {
            wheelMoveDirection = -1.0f;
        }

        auto radius = m_Scene->m_ModelsBoundingSphere.w;
        m_CameraScaleSpeed = radius * 0.05;

        auto moveSize =
                static_cast<float>(-wheelMoveDirection * m_CameraScaleSpeed);
        m_Camera->moveZ(moveSize);

        UpdateCameraMoveSpeed(m_Scene->m_ModelsBoundingSphere);
    };

protected:
    BasicInteractor() {};
    ~BasicInteractor() override = default;

    void ModelRotation() {
        igm::vec3 oldPoint3D, newPoint3D;
        MapToSphere(oldPoint3D, newPoint3D);

        igm::vec3 axis = igm::cross(oldPoint3D, newPoint3D); // corss product
        if (axis.length() < 1e-7) {
            axis = igm::vec3(1.0f, 0.0f, 0.0f);
        } else {
            axis.normalize();
        }
        // find the amount of rotation
        igm::vec3 d = oldPoint3D - newPoint3D;
        const double trackballradius = 0.6;
        double t = 0.5 * d.length() / trackballradius;
        if (t < -1.0) {
            t = -1.0;
        } else if (t > 1.0) {
            t = 1.0;
        }

        double phi = 2.0 * asin(t);
        double angle = phi * 180.0 / IGM_PI;

        igm::vec4 center = igm::vec4{m_Scene->m_ModelsBoundingSphere.xyz(), 1.0f};
        igm::vec3 centerInWorld = (m_Scene->m_ModelMatrix * center).xyz();

        igm::mat4 translateToOrigin = igm::translate(igm::mat4{}, -centerInWorld);
        igm::mat4 translateBack = igm::translate(igm::mat4{}, centerInWorld);
        igm::mat4 rotateMatrix = igm::rotate(
                igm::mat4{}, static_cast<float>(igm::radians(angle)), axis);

        igm::mat4 rotate = translateBack * rotateMatrix * translateToOrigin;
        m_Scene->m_ModelMatrix = rotate * (m_Scene->m_ModelMatrix);

        // updated the rotation matrix of the origin
        m_Scene->m_ModelRotate = rotateMatrix * (m_Scene->m_ModelRotate);
    };

    void ViewTranslation() {
        if (m_Camera) {
            auto offset = m_NewPoint2D - m_OldPoint2D;
            UpdateCameraMoveSpeed(m_Scene->m_ModelsBoundingSphere);
            m_Camera->moveXY(-offset.x * m_CameraMoveSpeed,
                             offset.y * m_CameraMoveSpeed);
        }
    };

    void MapToSphere(igm::vec3& old_v3D, igm::vec3& new_v3D) {
        auto center = igm::vec3(m_Scene->m_ModelsBoundingSphere);

        igm::mat4 model = m_Scene->m_ModelMatrix;
        igm::mat4 view = m_Camera->GetViewMatrix();
        igm::mat4 proj = m_Camera->GetProjectionMatrixReversedZ();

        auto p = igm::vec4{center, 1.0f};
        auto p_mvp = (proj * view * model * p);
        p_mvp /= p_mvp.w;

        // if the perspective enters the model, rotate around (0,0)
        if (p_mvp.x > 1.0f || p_mvp.x < 0.0f || p_mvp.y > 1.0f || p_mvp.y < 0.0f) {
            p_mvp = igm::vec4{0.0f, 0.0f, 0.0f, 0.0f};
        }

        auto width = m_Camera->GetViewPort().x;
        auto height = m_Camera->GetViewPort().y;

        const double trackballradius = 0.6;
        const double rsqr = trackballradius * trackballradius;

        // calculate old hit sphere point3D
        double oldX = (2.0 * m_OldPoint2D.x - width) / width - p_mvp.x;
        double oldY = -(2.0 * m_OldPoint2D.y - height) / height - p_mvp.y;
        double old_x2y2 = oldX * oldX + oldY * oldY;

        old_v3D[0] = oldX;
        old_v3D[1] = oldY;
        if (old_x2y2 < 0.5 * rsqr) {
            old_v3D[2] = sqrt(rsqr - old_x2y2);
        } else {
            old_v3D[2] = 0.5 * rsqr / sqrt(old_x2y2);
        }

        // calculate new hit sphere point3D
        double newX = (2.0 * m_NewPoint2D.x - width) / width - p_mvp.x;
        double newY = -(2.0 * m_NewPoint2D.y - height) / height - p_mvp.y;
        double new_x2y2 = newX * newX + newY * newY;

        new_v3D[0] = newX;
        new_v3D[1] = newY;
        if (new_x2y2 < 0.5 * rsqr) {
            new_v3D[2] = sqrt(rsqr - new_x2y2);
        } else {
            new_v3D[2] = 0.5 * rsqr / sqrt(new_x2y2);
        }
    };

    void UpdateCameraMoveSpeed(const igm::vec4& center) {
        auto viewport = m_Camera->GetViewPort();
        auto viewportF = igm::vec2{static_cast<float>(viewport.x),
                                   static_cast<float>(viewport.y)};

        igm::mat4 model = m_Scene->m_ModelMatrix;
        igm::mat4 view = m_Camera->GetViewMatrix();
        igm::mat4 proj = m_Camera->GetProjectionMatrixReversedZ();
        auto mvp = proj * view * model;

        auto boundingCenterMvp = mvp * igm::vec4{center.xyz(), 1.0f};
        boundingCenterMvp /= boundingCenterMvp.w;
        auto bz = boundingCenterMvp.z;

        // the center of the bounding-sphere is located behind the near plane
        if (bz > 1.0f || bz < 0.0f) {
            auto cameraFront = m_Camera->GetCameraFront().normalized();
            auto boundingBehind = center.xyz() + cameraFront * center.w;
            auto boundingBehindMvp = mvp * igm::vec4{boundingBehind, 1.0f};
            boundingBehindMvp /= boundingBehindMvp.w;
            bz = boundingBehindMvp.z;

            // the bounding-sphere is behind camera
            if (bz > 1.0f) return;
        }

        Eigen::Matrix4f A;
        Eigen::Vector4f b;
        A << mvp[0][0], mvp[1][0], mvp[2][0], mvp[3][0], mvp[0][1], mvp[1][1],
                mvp[2][1], mvp[3][1], mvp[0][2], mvp[1][2], mvp[2][2], mvp[3][2],
                mvp[0][3], mvp[1][3], mvp[2][3], mvp[3][3];

        igm::vec3 c{0.0f};
        // c is the point located at the center of the screen after mvp transformation
        {
            // | x11 x12 x13 x14 |   |  x  | =  |  0  |
            // | x21 x22 x23 x24 | * |  y  |    |  0  |
            // | x31 x32 x33 x34 |   |  z  |    | w*bz|
            // | x41 x42 x43 x44 |   | 1.0 |    |  w  |
            b << 0.0f, 0.0f, bz, 1.0f;

            Eigen::Vector4f solution = A.colPivHouseholderQr().solve(b);
            float x = solution(0);
            float y = solution(1);
            float z = solution(2);
            float w = solution(3);
            c = igm::vec3{x / w, y / w, z / w};
        }

        igm::vec3 p{0.0f};
        // p is the point located at the screen pixel (0,1) after mvp transformation
        {
            // | x11 x12 x13 x14 |   |  x  | =  |        0       |
            // | x21 x22 x23 x24 | * |  y  |    | w * 2 / height |
            // | x31 x32 x33 x34 |   |  z  |    |      w * bz    |
            // | x41 x42 x43 x44 |   | 1.0 |    |        w       |
            b << 0.0f, 2.0f / viewportF.y, bz, 1.0f;

            Eigen::Vector4f solution = A.colPivHouseholderQr().solve(b);
            float x = solution(0);
            float y = solution(1);
            float z = solution(2);
            float w = solution(3);
            p = igm::vec3{x / w, y / w, z / w};
        }

        m_CameraMoveSpeed = (p - c).length();
    };
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H