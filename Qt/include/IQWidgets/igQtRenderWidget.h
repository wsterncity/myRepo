/**
 * @class   iGameQtGLFWWindow
 * @brief   Provides Qt window and context support  for external renderers
 */

#pragma once

#include "iGameSceneManager.h"
#include "iGameInteractor.h"

#include <IQCore/igQtExportModule.h>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QOpenGLWidget>

using namespace iGame;

class IG_QT_MODULE_EXPORT igQtRenderWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    igQtRenderWidget(QWidget* parent = nullptr);
    ~igQtRenderWidget() override;

    Scene* GetScene() {
        return m_Scene.get();
    }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;


    Scene::Pointer m_Scene;
    Interactor::Pointer m_Interactor;

 public slots:
     void MakeCurrent()
     {
         makeCurrent();
     }
     void DoneCurrent()
     {
         doneCurrent();
     }

     void ChangeViewStyle(int index)
     {
         if (m_Scene->GetCurrentObject())
         {
             m_Scene->GetCurrentObject()->SetViewStyleOfModel(index);
         }
         update();
     }
     void ChangeScalarView(int index, int dim = -1)
     {
         makeCurrent();
         if (m_Scene->GetCurrentObject())
         {
             m_Scene->GetCurrentObject()->ViewCloudPictureOfModel(index - 1, dim - 1);
         }
         doneCurrent();
         update();
     }
};
