/**
 * @class   iGameQtGLFWWindow
 * @brief   Provides Qt window and context support  for external renderers
 */

#pragma once

#include <iGameSmartPointer.h>

#ifdef __APPLE__
#define __gl3_h_
#define __glext_h_
#define __glext3_h_
#endif

#include <IQCore/igQtExportModule.h>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QOpenGLWidget>

namespace iGame
{
class Scene;
class DataObject;

class Interactor;
} // namespace iGame

using namespace iGame;

class IG_QT_MODULE_EXPORT igQtRenderWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    igQtRenderWidget(QWidget* parent = nullptr);
    ~igQtRenderWidget() override;

    Scene* GetScene();

    void AddDataObject(SmartPointer<DataObject> obj);
    void ChangeInteractor(SmartPointer<Interactor> it);
    void update() { QOpenGLWidget::update(); }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    SmartPointer<Scene> m_Scene;
    SmartPointer<Interactor> m_Interactor;

public slots:
    void MakeCurrent() { makeCurrent(); }
    void DoneCurrent() { doneCurrent(); }

    void ChangeViewStyle(int index);
    void ChangeScalarView(int index, int dim = -1);

signals:
    void AddDataObjectToModelList(QString model_name);
    void UpdateCurrentDataObject();
};
