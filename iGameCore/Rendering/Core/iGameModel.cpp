#include "iGameModel.h"
#include "iGameScene.h"
#include "iGameFilter.h"
#include "iGameInteractor.h"


IGAME_NAMESPACE_BEGIN
void Model::Draw(Scene* scene) {
    GLCheckError();
    m_DataObject->Draw(scene);
    GLCheckError();
    m_PickedPointPainter->Draw(scene);
    GLCheckError();
    m_PickedLinePainter->Draw(scene);
    GLCheckError();
    m_PickedFacePainter->Draw(scene);
    GLCheckError();
    m_BBoxPainter->Draw(scene);
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
    if (GetSwitch(ViewSwitch::BoundingBox)) {
        m_BBoxPainter->SetVisibility(true);
    }

    if (GetSwitch(ViewSwitch::PickedItem)) {
        m_PickedPointPainter->SetVisibility(true);
        m_PickedLinePainter->SetVisibility(true);
        m_PickedFacePainter->SetVisibility(true);
    }
}

void Model::Hide() {
    m_DataObject->SetVisibility(false);
    m_BBoxPainter->SetVisibility(false);
    m_PickedPointPainter->SetVisibility(false);
    m_PickedLinePainter->SetVisibility(false);
    m_PickedFacePainter->SetVisibility(false);
    m_Scene->ChangeModelVisibility(this, false);
}

void Model::SetBoundingBoxSwitch(bool action) {
    if (action) {
        SwitchOn(ViewSwitch::BoundingBox);
        if (m_DataObject->GetVisibility()) {
            m_BBoxPainter->SetVisibility(true);
        }

        auto& bbox = m_DataObject->GetBoundingBox();

        double length = bbox.max[0] - bbox.min[0];
        double width = bbox.max[1] - bbox.min[1];
        double height = bbox.max[2] - bbox.min[2];

        Vector3d p1 = bbox.min;
        Vector3d p2(bbox.min[0] + length, bbox.min[1], bbox.min[2]);
        Vector3d p3(bbox.min[0] + length, bbox.min[1] + width, bbox.min[2]);
        Vector3d p4(bbox.min[0], bbox.min[1] + width, bbox.min[2]);
        Vector3d p5(bbox.min[0], bbox.min[1], bbox.min[2] + height);
        Vector3d p6(bbox.max[0], bbox.max[1] - width, bbox.max[2]);
        Vector3d p7 = bbox.max;
        Vector3d p8(bbox.max[0] - length, bbox.max[1], bbox.max[2]);

        m_BBoxPainter->Clear();

        m_BBoxPainter->DrawLine(p1, p2, Color::LightBlue);
        m_BBoxPainter->DrawLine(p2, p3, Color::LightBlue);
        m_BBoxPainter->DrawLine(p3, p4, Color::LightBlue);
        m_BBoxPainter->DrawLine(p4, p1, Color::LightBlue);

        m_BBoxPainter->DrawLine(p1, p5, Color::LightBlue);
        m_BBoxPainter->DrawLine(p2, p6, Color::LightBlue);
        m_BBoxPainter->DrawLine(p3, p7, Color::LightBlue);
        m_BBoxPainter->DrawLine(p4, p8, Color::LightBlue);

        m_BBoxPainter->DrawLine(p5, p6, Color::LightBlue);
        m_BBoxPainter->DrawLine(p6, p7, Color::LightBlue);
        m_BBoxPainter->DrawLine(p7, p8, Color::LightBlue);
        m_BBoxPainter->DrawLine(p8, p5, Color::LightBlue);

    } else {
        SwitchOff(ViewSwitch::BoundingBox);
        m_BBoxPainter->SetVisibility(false);
    }
}

void Model::SetPickedItemSwitch(bool action) {
    if (action) {
        SwitchOn(ViewSwitch::PickedItem);
        if (m_DataObject->GetVisibility()) {
            m_PickedPointPainter->SetVisibility(true);
            m_PickedLinePainter->SetVisibility(true);
            m_PickedFacePainter->SetVisibility(true);
        }
    } else {
        SwitchOff(ViewSwitch::PickedItem);
        m_PickedPointPainter->SetVisibility(false);
        m_PickedLinePainter->SetVisibility(false);
        m_PickedFacePainter->SetVisibility(false);
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
	m_PickedPointPainter = PointPainter::New();
    m_PickedPointPainter->SetPointSize(6);
    m_PickedLinePainter = LinePainter::New();
    m_PickedLinePainter->SetLineWidth(2);
    m_PickedFacePainter = FacePainter::New();
    m_BBoxPainter = LinePainter::New();
    m_BBoxPainter->SetLineWidth(2);

    SwitchOff(ViewSwitch::BoundingBox);
    SwitchOn(ViewSwitch::PickedItem);
}
IGAME_NAMESPACE_END
