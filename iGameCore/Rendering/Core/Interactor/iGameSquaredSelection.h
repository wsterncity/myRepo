#ifndef OPENIGAME_SQUARED_SELECTION_INTERACTOR_H
#define OPENIGAME_SQUARED_SELECTION_INTERACTOR_H

#include "iGameBasicInteractor.h"
#include "iGamePointPicker.h"
#include "iGamePointSet.h"

IGAME_NAMESPACE_BEGIN
class SquaredSelection : public Interactor {
public:
    I_OBJECT(SquaredSelection);
    static Pointer New() { return new SquaredSelection; }


    void MousePressEvent(int posX, int posY, MouseButton mouseMode) override {
        m_OldPoint2D = {float(posX), (float) posY};
        m_MouseMode = mouseMode;

        // Screen coordinate
        width = (float) m_Camera->GetViewPort().x;
        height = (float) m_Camera->GetViewPort().y;

        mvp_invert =
                (m_Scene->CameraData().proj_view * m_Scene->ObjectData().model)
                        .invert();
    }


    void MouseMoveEvent(int posX, int posY) override {
        m_NewPoint2D = {float(posX), (float) posY};
        switch (m_MouseMode) {
            case iGame::LeftButton: {
                float x1 = m_OldPoint2D.x;
                float y1 = m_OldPoint2D.y;
                float x2 = m_NewPoint2D.x;
                float y2 = m_NewPoint2D.y;
                if (x2 < x1) { std::swap(x1, x2); }
                if (y2 < y1) { std::swap(y1, y2); }

                igm::vec3 leftTopPoint =
                        GetNearWorldCoord(igm::vec2(x1, y1), mvp_invert);
                igm::vec3 leftButtomPoint =
                        GetNearWorldCoord(igm::vec2(x1, y2), mvp_invert);
                igm::vec3 rightTopPoint =
                        GetNearWorldCoord(igm::vec2(x2, y1), mvp_invert);
                igm::vec3 rightButtomPoint =
                        GetNearWorldCoord(igm::vec2(x2, y2), mvp_invert);

                igm::vec3 leftTopInterPoint =
                        GetFarWorldCoord(igm::vec2(x1, y1), mvp_invert);
                igm::vec3 rightButtomInterPoint =
                        GetFarWorldCoord(igm::vec2(x2, y2), mvp_invert);

                igm::vec3 leftNormal =
                        (leftButtomPoint - leftTopPoint)
                                .cross(leftTopInterPoint - leftTopPoint);
                igm::vec4 leftPlane = GetPlane(leftTopPoint, leftNormal);
                igm::vec3 topNormal =
                        (leftTopInterPoint - leftTopPoint)
                                .cross(rightTopPoint - leftTopPoint);
                igm::vec4 topPlane = GetPlane(leftTopPoint, topNormal);
                igm::vec3 rightNormal = (rightTopPoint - rightButtomPoint)
                                                .cross(rightButtomInterPoint -
                                                       rightButtomPoint);
                igm::vec4 rightPlane = GetPlane(rightButtomPoint, rightNormal);
                igm::vec3 bottomNormal =
                        (rightButtomInterPoint - rightButtomPoint)
                                .cross(leftButtomPoint - rightButtomPoint);
                igm::vec4 butttomPlane =
                        GetPlane(rightButtomPoint, bottomNormal);
                igm::vec3 nearNormal =
                        (rightButtomPoint - rightTopPoint)
                                .cross(leftTopPoint - rightTopPoint);
                igm::vec4 nearPlane = GetPlane(rightTopPoint, nearNormal);
                igm::vec4 farPlane = GetPlane(leftTopInterPoint, -nearNormal);

                std::vector<igm::vec4> planes;
                planes.push_back(leftPlane);
                planes.push_back(topPlane);
                planes.push_back(rightPlane);
                planes.push_back(butttomPlane);
                planes.push_back(nearPlane);
                planes.push_back(farPlane);

                this->SelectElement(planes);
            } break;
            default:
                break;
        }
    }
    void MouseReleaseEvent(int posX, int posY) override {
        m_MouseMode = NoButton;
    }
    void WheelEvent(double delta) override {}

    virtual void SelectElement(const std::vector<igm::vec4>& planes) {};

    igm::vec4 GetPlane(const igm::vec3& p, const igm::vec3& normal) {
        igm::vec3 n = normal.normalized();
        return igm::vec4(n, -n.dot(p));
    }

    igm::vec3 GetNearWorldCoord(const igm::vec2& screenCoord,
                                const igm::mat4& invertedMvp) {
        igm::vec2 NDC(2.0f * screenCoord.x / width - 1.0f,
                      1.0f - (2.0f * screenCoord.y / height));

        // Clipping coordinate
        igm::vec4 clippingCoord(NDC, 1, 1.0);

        // World coordinate
        igm::vec4 worldCoord = invertedMvp * clippingCoord;
        return igm::vec3(worldCoord / worldCoord.w);
    }


    igm::vec3 GetFarWorldCoord(const igm::vec2& screenCoord,
                               const igm::mat4& invertedMvp) {
        igm::vec2 NDC(2.0f * screenCoord.x / width - 1.0f,
                      1.0f - (2.0f * screenCoord.y / height));

        // Clipping coordinate
        igm::vec4 clippingCoord(NDC, 0.001, 1.0);

        // World coordinate
        igm::vec4 worldCoord = invertedMvp * clippingCoord;
        return igm::vec3(worldCoord / worldCoord.w);
    }

protected:
    SquaredSelection() {}
    ~SquaredSelection() override = default;

    float width{}, height{};
    igm::mat4 mvp_invert{};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H