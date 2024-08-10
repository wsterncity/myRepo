#include "iGameModel.h"
#include "iGameScene.h"


IGAME_NAMESPACE_BEGIN
void Model::Draw(Scene* scene) 
{
	m_DataObject->Draw(scene);
    m_PickedPointPainter->Draw(scene);
    m_PickedLinePainter->Draw(scene);
    m_BoundingBoxPainter->Draw(scene);
}

void Model::Update() {
    if (m_Scene) {
        m_Scene->Update();
    }
}

Model::Model() {
	m_PickedPointPainter = PointPainter::New();
    m_PickedPointPainter->SetPointSize(8);
    m_PickedLinePainter = LinePainter::New();
    m_PickedLinePainter->SetLineWidth(2);
    m_BoundingBoxPainter = LinePainter::New();
    m_BoundingBoxPainter->SetLineWidth(2);
}
IGAME_NAMESPACE_END


