#include "iGameInteractor.h"

IGAME_NAMESPACE_BEGIN

Interactor::Interactor() {
    m_MouseMode = NoButton;
    m_OldPoint2D = {0.0f, 0.0f};
}
Interactor::~Interactor() {}

void Interactor::MousePressEvent(int _eventX, int _eventY,
                                 MouseButton _mouseMode) {
    m_OldPoint2D = {float(_eventX), (float) _eventY};
    m_MouseMode = _mouseMode;
}

void Interactor::MouseMoveEvent(int _eventX, int _eventY) {
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
}

void Interactor::MouseReleaseEvent(int _eventX, int _eventY) {
    m_MouseMode = NoButton;
}

void Interactor::WheelEvent(double delta) {
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
    m_CameraScaleSpeed = radius * 0.1;

    auto moveSize =
            static_cast<float>(-wheelMoveDirection * m_CameraScaleSpeed);
    m_Camera->moveZ(moveSize);

    UpdateCameraMoveSpeed(m_Scene->m_ModelsBoundingSphere);
}

void Interactor::Initialize() { m_Camera = m_Scene->m_Camera; }

void Interactor::ModelRotation() {
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
}


void Interactor::ViewTranslation() {
    if (m_Camera) {
        auto offset = m_NewPoint2D - m_OldPoint2D;
        m_Camera->moveXY(-offset.x * m_CameraMoveSpeed,
                         offset.y * m_CameraMoveSpeed);
        UpdateCameraMoveSpeed(m_Scene->m_ModelsBoundingSphere);
    }
}

void Interactor::MapToSphere(igm::vec3& old_v3D, igm::vec3& new_v3D) {
    auto center = igm::vec3(m_Scene->m_ModelsBoundingSphere);

    igm::mat4 model = m_Scene->m_ModelMatrix;
    igm::mat4 view = m_Camera->GetViewMatrix();
    igm::mat4 proj = m_Camera->GetProjectionMatrixReversedZ();

    auto p = igm::vec4{center, 1.0f};
    auto p_mvp = (proj * view * model * p);
    p_mvp /= p_mvp.w;

    int width = m_Camera->GetViewPort().x / m_Camera->GetDevicePixelRatio();
    int height = m_Camera->GetViewPort().y / m_Camera->GetDevicePixelRatio();

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
}

void Interactor::UpdateCameraMoveSpeed(const igm::vec4& _center) {
    igm::mat4 model = m_Scene->m_ModelMatrix;
    igm::mat4 view = m_Camera->GetViewMatrix();
    igm::mat4 proj = m_Camera->GetProjectionMatrixReversedZ();

    // update camera movement speed to adapt to pan
    auto center = _center.xyz();
    auto radius = _center.w;

    auto centerInWorld = (model * igm::vec4{center, 1.0f}).xyz();
    auto centerUpInWorld =
            centerInWorld + m_Camera->GetCameraUp().normalize() * radius;
    auto centerInWorldHomogeneous = igm::vec4{centerInWorld, 1.0f};
    auto centerUpInWorldHomogeneous = igm::vec4{centerUpInWorld, 1.0f};

    // no need to multiply the model on the left
    auto p1_mvp = (proj * view * centerInWorldHomogeneous);
    p1_mvp /= p1_mvp.w;
    auto p2_mvp = (proj * view * centerUpInWorldHomogeneous);
    p2_mvp /= p2_mvp.w;

    auto height = m_Camera->GetViewPort().y /
                  static_cast<float>(m_Camera->GetDevicePixelRatio());
    auto acturalPixel = igm::vec2(p1_mvp - p2_mvp).length() * height / 2.0f;

    m_CameraMoveSpeed = radius / acturalPixel;
}

IGAME_NAMESPACE_END