//
// Created by m_ky on 2024/5/22.
//

/**
 * @class   igQtFilterDialogDockWidget
 * @brief   igQtFilterDialogDockWidget's brief
 */
#include <IQComponents/igQtFilterDialogDockWidget.h>

igQtFilterDialogDockWidget::igQtFilterDialogDockWidget(QWidget *parent) : QDockWidget(parent), ui(new Ui::FilterDockDialog) {
    ui->setupUi(this);
}

igQtFilterDialogDockWidget::~igQtFilterDialogDockWidget() {

}

