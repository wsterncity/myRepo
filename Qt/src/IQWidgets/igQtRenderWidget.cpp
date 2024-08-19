//
// Created by m_ky on 2024/4/18.
//

/**
 * @class   iGameQtGLFWWindow
 * @brief   iGameQtGLFWWindow's brief
 */
#include "iGameSceneManager.h"
#include "Core/Interactor/iGameInteractor.h"

#include <IQWidgets/igQtRenderWidget.h>
#include <qdebug.h>
#include <QMouseEvent>

igQtRenderWidget::igQtRenderWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    setMouseTracking(true);
    setMinimumHeight(185);
    setMinimumWidth(320);
}

igQtRenderWidget::~igQtRenderWidget()
{
    makeCurrent();
    SceneManager::Pointer sceneManager = SceneManager::Instance();
    sceneManager->DeleteScene(m_Scene);
    m_Scene = nullptr;
    doneCurrent();
}

Scene* igQtRenderWidget::GetScene() { return m_Scene; }

void igQtRenderWidget::AddDataObject(SmartPointer<DataObject> obj) {
    //m_Scene->AddDataObject(obj);
    //Q_EMIT AddDataObjectToModelList(QString::fromStdString(obj->GetName()));
    //update();
}

void igQtRenderWidget::ChangeInteractor(SmartPointer<Interactor> it) {
    m_Interactor = it;
    m_Interactor->SetScene(m_Scene);
}

void igQtRenderWidget::initializeGL()
{
    // 目前当窗口
    SceneManager::Pointer sceneManager = SceneManager::Instance();
    m_Scene = sceneManager->NewScene();
    m_Scene->SetUpdateFunctor(&igQtRenderWidget::update, this);
    m_Scene->SetMakeCurrentFunctor(&igQtRenderWidget::makeCurrent, this);
    m_Scene->SetDoneCurrentFunctor(&igQtRenderWidget::doneCurrent, this);
    m_Interactor = Interactor::New();
    m_Interactor->SetScene(m_Scene);
}

void igQtRenderWidget::resizeGL(int w, int h)
{
    auto ratio = this->devicePixelRatio();
    m_Scene->Resize(width(), height(), ratio);
}

void igQtRenderWidget::paintGL()
{
    m_Scene->Draw();
}


void igQtRenderWidget::mousePressEvent(QMouseEvent* event)
{
    MouseButton mode{};
    switch (event->button()) {
    case Qt::NoButton:
        mode = MouseButton::NoButton;
        break;
    case Qt::LeftButton:
        mode = MouseButton::LeftButton;
        break;
    case Qt::RightButton:
        mode = MouseButton::RightButton;
        break;
    default:
        break;
    }
    m_Interactor->MousePressEvent(event->pos().x(), event->pos().y(), mode);
    update();
}

void igQtRenderWidget::mouseMoveEvent(QMouseEvent* event)
{
    m_Interactor->MouseMoveEvent(event->pos().x(), event->pos().y());
    update();
}

void igQtRenderWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_Interactor->MouseReleaseEvent(event->pos().x(), event->pos().y());
    update();
}

void igQtRenderWidget::wheelEvent(QWheelEvent* event)
{
    m_Interactor->WheelEvent(event->delta());
    update();
}

//void igQtRenderWidget::ChangeViewStyle(int index) {
//    if (m_Scene->GetCurrentObject()) {
//    m_Scene->GetCurrentObject()->SetViewStyleOfModel(index);
//    }
//    update();
//}
//void igQtRenderWidget::ChangeScalarView(int index, int dim) {
//    makeCurrent();
//    if (m_Scene->GetCurrentObject()) {
//    m_Scene->GetCurrentObject()->ViewCloudPictureOfModel(index - 1, dim);
//    }
//    doneCurrent();
//    update();
//}