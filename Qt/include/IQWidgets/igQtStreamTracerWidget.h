#pragma once
#include<ui_igstreamtracer.h>
//#include <iGameManager.h>
#include<iostream>
#include<iGameStreamTracer.h>
#include <iGameUnstructuredMesh.h>
class igQtStreamTracerWidget : public QWidget {

    Q_OBJECT

public:
    igQtStreamTracerWidget(QWidget* parent = nullptr);
public slots:
	void generateStreamline();
    void changeControl();
    void changeProportion();
    void reduceProportion();
    void increaseProportion();
    void changenumOfSeeds();
    void changelengthOfStreamLine();
    void changelengthOfStep();
    void changemaxSteps();
    void changeterminalSpeed();
    void Pressed();
    void Released();
signals:
    void NewModel(DataObject::Pointer obj, ItemSource source);
    void UpdateModel(DataObject::Pointer obj, ItemSource source);
    void sendstreams();
    void updatestreams();
private:
    Ui::SteamLineTracer* ui;
    int numOfSeeds;
    int control;
    float proportion;
    float lengthOfStreamLine;
    float lengthOfStep;
    float maxSteps;
    float terminalSpeed;
    bool haveDraw;
    bool haveClicked;
    UnstructuredMesh::Pointer streamlineResult{};
};
