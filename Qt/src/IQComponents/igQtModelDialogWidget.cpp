#include<iGameSceneManager.h>

#include <IQComponents/igQtModelDialogWidget.h>
#include <qdebug.h>
#include <QQueue>
#include <qmenu.h>
#include <qaction.h>
#include <QSplitter>

igQtModelDialogWidget::igQtModelDialogWidget(QWidget* parent)
	: QDockWidget(parent),
	ui(new Ui::LayerDialog)
{
	ui->setupUi(this);
    this->setMinimumWidth(parent->width() / 4);

    QSplitter* splitter = new QSplitter(Qt::Vertical, this);
    this->setWidget(splitter);
    splitter->addWidget(ui->modelTreeWidget);
    splitter->addWidget(ui->propertyTreeWidget);


	modelTreeWidget = ui->modelTreeWidget;
	modelTreeWidget->setColumnCount(2);
    modelTreeWidget->header()->hide();
    modelTreeWidget->setColumnWidth(0, 150);

    propertyTreeWidget = ui->propertyTreeWidget;

    //this->setStyleSheet(
    //    "QtPropertyEditorView{"
    //    "border: 1px solid #a0a0a0;"
    //    "background - color: red;"
    //    "}"
    //    "QtPropertyEditorView::item{"
    //    "border: none;"
    //    "background - color: red;"
    //    "margin - top: 1px;"
    //    "margin - bottom: 1px;"
    //    "padding: 5px;"
    //    "font - size: 14px;"
    //    "}"
    //    "QtPropertyEditorView::item:selected, QtPropertyEditorView::branch : selected{"
    //    "background - color: red;"
    //    "color: black;"
    //    "}"
    //    "QtPropertyEditorView QHeaderView::section{"
    //    "background - color: #c0c0c0;"
    //    "border: 1px;"
    //    "color: #292727;"
    //    "font - size: 18px;"
    //    "font - weight:bold;"
    //    "}"
    //);

    
    QtVariantPropertyManager* varManager = new QtVariantPropertyManager(propertyTreeWidget);
    QtVariantEditorFactory* editFactory = new QtVariantEditorFactory(propertyTreeWidget);
    propertyTreeWidget->setFactoryForManager(varManager, editFactory);

    QtProperty* groupItem1 = varManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("分组1"));
    
    QtVariantProperty* item = varManager->addProperty(QVariant::Int, QStringLiteral("Int: "));
    item->setValue(100);
    //item->setEnabled(false);
    groupItem1->addSubProperty(item);

    QtVariantProperty* item2 = varManager->addProperty(QVariant::Bool, QStringLiteral("Bool: "));
    item2->setValue(true);
    item->addSubProperty(item2);

    item = varManager->addProperty(QVariant::Double, QStringLiteral("Double: "));
    item->setValue(12.34);
    groupItem1->addSubProperty(item);

    item = varManager->addProperty(QVariant::String, QStringLiteral("String: "));
    item->setValue(QStringLiteral("TestTest"));
    groupItem1->addSubProperty(item);

    propertyTreeWidget->addProperty(groupItem1);
    
}

