#pragma once
#include<ui_igVector.h>
//#include <iGameManager.h>
#include<iostream>

class igQtVectorWidget : public QWidget {

    Q_OBJECT

public:
    igQtVectorWidget(QWidget* parent = nullptr);
public slots:
    void changeHR();
    void changeHL();
    void changeTR();
    void changeTL();


private:
    Ui::igVector* ui;
    float headRadius;
    float headLength;
    float tailRadius;
    float tailLength;
};