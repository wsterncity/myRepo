#pragma once

#include "igQtRenderWidget.h"

class igQtModelDrawWidget : public igQtRenderWidget {
    Q_OBJECT
public:
    igQtModelDrawWidget(QWidget* parent = nullptr);
    ~igQtModelDrawWidget() override;

protected:

    /* 没用可以删了，功能整合到 igQtRenderWight 上 */

private:
};
