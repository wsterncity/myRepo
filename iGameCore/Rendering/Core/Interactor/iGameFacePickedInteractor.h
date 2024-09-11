#ifndef OPENIGAME_FACE_PICKED_INTERACTOR_H
#define OPENIGAME_FACE_PICKED_INTERACTOR_H

#include "iGameInteractor.h"
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

    void SetFacesAndPainter(Points::Pointer points, CellArray::Pointer faces,
                            FacePainter::Pointer painter);

    void MousePressEvent(int posX, int posY, MouseButton mouseMode) override;
    void MouseMoveEvent(int posX, int posY) override;
    void MouseReleaseEvent(int posX, int posY) override;
    void WheelEvent(double delta) override;

protected:
    FacePickedInteractor() = default;
    ~FacePickedInteractor() override = default;

    bool IsIntersectTriangle(igm::vec3 orig, igm::vec3 end, igm::vec3 v0,
                             igm::vec3 v1, igm::vec3 v2,
                             igm::vec3& intersection);

    double DistancePointToPlane(igm::vec3 point, igm::vec3 p1, igm::vec3 p2,
                                igm::vec3 p3);

    Points::Pointer Points{};
    CellArray::Pointer Faces{};
    FacePainter::Pointer Painter{};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H