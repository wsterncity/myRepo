//
// Created by m_ky on 2024/5/22.
//

/**
 * @class   igQtFilterDialogDockWidget
 * @brief   igQtFilterDialogDockWidget's brief
 */
#pragma once

#include <ui_filterDialog.h>
#include <IQCore/igQtExportModule.h>

class IG_QT_MODULE_EXPORT igQtFilterDialogDockWidget : public QDockWidget {

    Q_OBJECT
public:
    igQtFilterDialogDockWidget(QWidget* parent = Q_NULLPTR);
    ~igQtFilterDialogDockWidget() override;

public:

public:
private slots:
private:
    Ui::FilterDockDialog* ui;
};

