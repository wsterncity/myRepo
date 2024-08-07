//
// Created by m_ky on 2024/5/22.
//

/**
 * @class   igQtFilterDialogDockWidget
 * @brief   igQtFilterDialogDockWidget's brief
 */
#include <IQComponents/igQtFilterDialogDockWidget.h>
#include <iostream>


igQtFilterDialogDockWidget::igQtFilterDialogDockWidget(QWidget *parent) : QDockWidget(parent), ui(new Ui::FilterDockDialog) {
    ui->setupUi(this);

    connect(ui->applyButton, &QPushButton::clicked, this, &igQtFilterDialogDockWidget::apply);
    connect(ui->closeButton, &QPushButton::clicked, this, &igQtFilterDialogDockWidget::close);

    QHBoxLayout* hLayout = new QHBoxLayout();
    titleList = new QVBoxLayout();
    valueList = new QVBoxLayout();

    hLayout->addLayout(titleList);
    hLayout->addLayout(valueList);

    titleList->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    valueList->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
    hLayout->setStretchFactor(titleList, 1);
    hLayout->setSpacing(10);
    hLayout->setStretchFactor(valueList, 1);

    titleList->setSpacing(4);
    valueList->setSpacing(4);

    ui->verticalLayout->setAlignment(Qt::AlignTop);
    ui->verticalLayout->setMargin(20);
    ui->verticalLayout->addLayout(hLayout);
    index = 0;
}

igQtFilterDialogDockWidget::~igQtFilterDialogDockWidget() {

}

void igQtFilterDialogDockWidget::apply() { 
    applyFunctor(); 
}

void igQtFilterDialogDockWidget::close() {
    hide();
    delete this;
}

void igQtFilterDialogDockWidget::setFilterTitle(const QString& title) {
    setWindowTitle(title);
}

void igQtFilterDialogDockWidget::setFilterDescription(const QString& text) {
    ui->filterInfoLabel->setText("    " + text);
}

int igQtFilterDialogDockWidget::addParameter(WidgetType type,
                                             const QString& title,
                                             const QString& defaultValue) {

    QWidget* widget = nullptr;
    
    switch (type) {
        case QT_LINE_EDIT:
        {
            QLineEdit* line = new QLineEdit(this);
            line->setText(defaultValue);
            widget = line;
        }
        break;
        case QT_CHECK_BOX: 
        {
            QCheckBox* check = new QCheckBox(this);
            bool value = defaultValue == "true" ? true : false;
            check->setChecked(value);
            widget = check;
        } break;
        default:
            break;
    }

    if (widget == nullptr) { 
        return (-1);
    }

    QLabel* label = new QLabel(this);
    label->setText(title);
    label->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    label->setMinimumHeight(20);
    widget->setMinimumHeight(20);

    titleList->addWidget(label);
    valueList->addWidget(widget);

    Item item{title, defaultValue, type, widget};
    itemMap[index] = item;
    return index++;
}