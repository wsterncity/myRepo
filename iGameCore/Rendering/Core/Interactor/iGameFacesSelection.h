#ifndef OPENIGAME_FACES_SELECTION_INTERACTOR_H
#define OPENIGAME_FACES_SELECTION_INTERACTOR_H

#include "iGameSquaredSelection.h"
#include "iGameSurfaceMesh.h"

IGAME_NAMESPACE_BEGIN
class FacesSelection : public SquaredSelection {
public:
    I_OBJECT(FacesSelection);
    static Pointer New() { return new FacesSelection; }

    void SetModel(Model* model);

    void SelectElement(const std::vector<igm::vec4>& planes) override;

    bool IsPointInFrustum(const igm::vec3& p,
                          const std::vector<igm::vec4>& planes);

protected:
    FacesSelection() = default;
    ~FacesSelection() override = default;

    SurfaceMesh* m_Mesh{nullptr};
    Model* m_Model{nullptr};
};
IGAME_NAMESPACE_END
#endif // OPENIGAME_INTERACTOR_H