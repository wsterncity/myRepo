#ifndef OPENIGAME_FACE_PICKED_INTERACTOR_H
#define OPENIGAME_FACE_PICKED_INTERACTOR_H

#include "iGameBasicInteractor.h"
#include "iGameSurfaceMesh.h"

IGAME_NAMESPACE_BEGIN
class FacePickedInteractor : public Interactor {
public:
    I_OBJECT(FacePickedInteractor);
    static Pointer New() { return new FacePickedInteractor; }

    struct Vector3Tovec3 {
        igm::vec3 operator()(const Vector3f& v) { 
            return igm::vec3(v[0], v[1], v[2]);
        }
        igm::vec3 operator()(const Vector3d& v) {
            return igm::vec3(v[0], v[1], v[2]);
        }
    };

    void SetFacesAndPainter(Points::Pointer points, CellArray::Pointer faces, FacePainter::Pointer painter) {
        Points = points;
        Faces = faces;
        Painter = painter;
    }

    void MousePressEvent(int posX, int posY, MouseButton mouseMode) override {
        igm::vec2 pos = {float(posX), (float) posY};
        if (Points == nullptr || Faces == nullptr || Painter == nullptr) { return; }

        auto width = (float) m_Camera->GetViewPort().x;
        auto height = (float) m_Camera->GetViewPort().y;

        // NDC coordinate [-1,1]
        float x = 2.0f * pos.x / width - 1.0f;
        float y = 1.0f - (2.0f * pos.y / height);

        auto mvp =
                (m_Scene->CameraData().proj_view * m_Scene->ObjectData().model);
        auto mvp_invert = mvp.invert();

        // Clipping coordinate
        igm::vec4 point(x, y, 1.0, 1);
        igm::vec4 pointEnd(x, y, 0.001, 1);

        // World coordinate
        igm::vec4 tpoint = mvp_invert * point;
        igm::vec4 tpointEnd = mvp_invert * pointEnd;

        // 3D World coordinate
        igm::vec3 point1(tpoint / tpoint.w);
        igm::vec3 point2(tpointEnd / tpointEnd.w);

        igm::vec3 dir = (point1 - point2).normalized();

        Vector3Tovec3 t;
        double zMax = -std::numeric_limits<double>::max();
        igIndex id = -1;
        for (int i = 0; i < Faces->GetNumberOfCells(); i++) {
            igIndex face[16]{};
            int size = Faces->GetCellIds(i, face);

            bool flag = false;
            auto& p0 = Points->GetPoint(face[0]);
            for (int j = 2; j < size; j++) { 
                auto& p1 = Points->GetPoint(face[j - 1]);
                auto& p2 = Points->GetPoint(face[j]);
                igm::vec3 intersect;
                if (IsIntersectTriangle(point1, point2, t(p0), t(p1), t(p2),
                    intersect)) {
                    flag = true;
                    break;
                }
            }

            if (flag) {
                igm::vec3 center(0);
                for (int j = 0; j < size; j++) {
                    auto& p = Points->GetPoint(face[j]);
                    center += t(p);
                }
                center /= size;
                igm::vec4 fDepth = mvp * igm::vec4(center, 1.0f);
                fDepth /= fDepth.w;

                if (zMax < fDepth.z && fDepth.z > 0 && fDepth.z < 1.0) {
                    id = i;
                    zMax = fDepth.z;
                }
            }
        }

        Painter->Clear();
        if (id != -1)
        {
            std::cout << "pick face id: " << id << std::endl;
            igIndex face[16]{};
            int size = Faces->GetCellIds(id, face);
            auto& p0 = Points->GetPoint(face[0]);
            for (int j = 2; j < size; j++) {
                auto& p1 = Points->GetPoint(face[j - 1]);
                auto& p2 = Points->GetPoint(face[j]);
                Painter->DrawTriangle(p0, p1, p2);
            }
        }
    }
    void MouseMoveEvent(int posX, int posY) override {}
    void MouseReleaseEvent(int posX, int posY) override {}
    void WheelEvent(double delta) override {}

protected:
    FacePickedInteractor() {}
    ~FacePickedInteractor() override = default;

    bool IsIntersectTriangle(igm::vec3 orig, igm::vec3 end, igm::vec3 v0,
                             igm::vec3 v1, igm::vec3 v2,
                             igm::vec3& intersection) {
        // Refer to https://www.cnblogs.com/graphics/archive/2010/08/09/1795348.html
        float t, u, v;
        igm::vec3 dir = end - orig;
        igm::vec3 E1 = v1 - v0;
        igm::vec3 E2 = v2 - v0;
        // cross product
        igm::vec3 P(dir.y * E2.z - E2.y * dir.z,
                    E2.x * dir.z - E2.z * dir.x,
                    dir.x * E2.y - dir.y * E2.x);
        // dot product
        float det = E1.x * P.x + E1.y * P.y + E1.z * P.z;
        igm::vec3 T;
        if (det > 0) {
            T = orig - v0;
        } else {
            T = v0 - orig;
            det = -det;
        }
        if (det < 0) { return false; }

        // Calculate u and make sure u <= 1
        u = T.x * P.x + T.y * P.y + T.z * P.z;
        if (u < 0.0f || u > det) { return false; }

        igm::vec3 Q(T.y * E1.z - E1.y * T.z,
                    E1.x * T.z - E1.z * T.x,
                    T.x * E1.y - T.y * E1.x);

        // Calculate v and make sure u + v <= 1
        v = dir.x * Q.x + dir.y * Q.y + dir.z * Q.z;
        if (v < 0.0f || u + v > det) { return false; }

        // Calculate t, scale parameters, ray intersects triangle
        float fInvDet = 1.0f / det;
        u *= fInvDet;
        v *= fInvDet;
        intersection = E1 * u + E2 * v + v0;
        return true;
    }


    double DistancePointToPlane(igm::vec3 point, igm::vec3 p1, igm::vec3 p2,
                                igm::vec3 p3) {
        // Calculate two vectors on the plane
        igm::vec3 v1 = {p2.x - p1.x, p2.y - p1.y, p2.z - p1.z};
        igm::vec3 v2 = {p3.x - p1.x, p3.y - p1.y, p3.z - p1.z};

        // Calculate the normal vector of the plane
        igm::vec3 n = v1.cross(v2);

        // Calculate the d constant in the plane equation
        double d = -n.dot(p1);

        // Calculate the distance from the point to the plane
        double numerator =
                std::abs(n.x * point.x + n.y * point.y + n.z * point.z + d);
        double denominator = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);

        return numerator / denominator;
    }

    Points::Pointer Points{};
    CellArray::Pointer Faces{};
    FacePainter::Pointer Painter{};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H