#include "iGameModel.h"
#include "iGameScene.h"
#include "iGameFilter.h"
#include "iGameInteractor.h"


IGAME_NAMESPACE_BEGIN
void Model::Draw(Scene* scene) {
    GLCheckError();
    m_DataObject->Draw(scene);
    GLCheckError();
    m_Painter->Draw(scene);
    GLCheckError();
}

void Model::Update() {
    if (m_Scene) { m_Scene->Update(); }
}

Selection* Model::GetSelection() { 
    if (m_Selection == nullptr) {
        m_Selection = Selection::New();
    }
    return m_Selection.get(); 
}

void Model::RequestPointSelection(Points* p, Selection* s) {
    if (m_Scene->GetInteractor() == nullptr) return;
    s->m_Points = p;
    s->m_Model = this;
    m_Scene->GetInteractor()->RequestPointSelectionStyle(s);
}

void Model::RequestDragPoint(Points* p, Selection* s) {
    if (m_Scene->GetInteractor() == nullptr) return;
    s->m_Points = p;
    s->m_Model = this;
    m_Scene->GetInteractor()->RequestDragPointStyle(s);
}

Filter* Model::GetModelFilter() { return m_Filter; }
void Model::DeleteModelFilter() { m_Filter = nullptr; }
void Model::SetModelFilter(SmartPointer<Filter> _filter) { m_Filter = _filter; }

void Model::Show() {
    m_DataObject->SetVisibility(true);
    m_Scene->ChangeModelVisibility(this, true);
    m_Painter->ShowAll();
}

void Model::Hide() {
    m_DataObject->SetVisibility(false);
    m_Scene->ChangeModelVisibility(this, false);
    m_Painter->HideAll();
}

void Model::SetBoundingBoxSwitch(bool action) {
    if (action) {
        SwitchOn(ViewSwitch::BoundingBox);

        auto& bbox = m_DataObject->GetBoundingBox();
        Vector3d p1 = bbox.min;
        Vector3d p7 = bbox.max;

        if (m_BboxHandle != 0) { m_Painter->Delete(m_BboxHandle); }
        m_Painter->SetPen(5);
        m_Painter->SetPen(Color::LightBlue);
        m_Painter->SetBrush(Color::None);
        m_BboxHandle = m_Painter->DrawCube(p1, p7);
    } else {
        SwitchOff(ViewSwitch::BoundingBox);
        m_Painter->Hide(m_BboxHandle);
    }
}

void Model::SetPickedItemSwitch(bool action) {
    if (action) {
        SwitchOn(ViewSwitch::PickedItem);
        if (m_DataObject->GetVisibility()) { m_Painter->ShowAll(); }
    } else {
        SwitchOff(ViewSwitch::PickedItem);
        m_Painter->HideAll();
    }
}

void Model::SetViewPointsSwitch(bool action) {
    if (action) {
        m_DataObject->AddViewStyle(IG_POINTS);
    } else {
        m_DataObject->RemoveViewStyle(IG_POINTS);
    }
}

void Model::SetViewWireframeSwitch(bool action) {
    if (action) {
        m_DataObject->AddViewStyle(IG_WIREFRAME);
    } else {
        m_DataObject->RemoveViewStyle(IG_WIREFRAME);
    }
}

void Model::SetViewFillSwitch(bool action) {
    if (action) {
        m_DataObject->AddViewStyle(IG_SURFACE);
    } else {
        m_DataObject->RemoveViewStyle(IG_SURFACE);
    }
}

Model::Model() {
    m_Painter = Painter::New();

    SwitchOff(ViewSwitch::BoundingBox);
    SwitchOn(ViewSwitch::PickedItem);
}
IGAME_NAMESPACE_END
