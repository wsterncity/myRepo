#ifndef OPENIGAME_FACE_PICKED_INTERACTOR_H
#define OPENIGAME_FACE_PICKED_INTERACTOR_H

#include "iGameInteractor.h"
#include "iGameSurfaceMesh.h"

IGAME_NAMESPACE_BEGIN
class FacePickedInteractor : public Interactor {
public:
    I_OBJECT(FacePickedInteractor);
    static Pointer New() { return new FacePickedInteractor; }

    void MousePressEvent(int posX, int posY, MouseButton mouseMode) override;
    void MouseMoveEvent(int posX, int posY) override;
    void MouseReleaseEvent(int posX, int posY) override;
    void WheelEvent(double delta) override;

    void SetModel(Model* model);

protected:
    FacePickedInteractor() = default;
    ~FacePickedInteractor() override = default;

    SurfaceMesh* m_Mesh{nullptr};
    Model* m_Model{nullptr};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H