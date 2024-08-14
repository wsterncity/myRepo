#include<iGameSceneManager.h>

#include <Plugin/qtpropertybrowser/qtpropertymanager.h>
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
    splitter->setChildrenCollapsible(false);

	modelTreeWidget = ui->modelTreeWidget;
    propertyTreeWidget = ui->propertyTreeWidget;

    modelTreeWidget->setColumnCount(2);
    modelTreeWidget->header()->hide();
    modelTreeWidget->setColumnWidth(0, 150);
    modelTreeWidget->setMinimumHeight(100);
    propertyTreeWidget->setMinimumHeight(100);
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
    connect(modelTreeWidget, &igQtModelTreeWidget::ChangeCurrentModel, this, [&](iGame::Model* model){
        propertyTreeWidget->clear();
//        model->GetDataObject()->
        model->GetDataObject()->GetMetadata();
        std::cout << model << '\n';
//        varManager->clear();
    });


    propertyTreeWidget->setFactoryForManager(varManager, editFactory);

    QtProperty* groupItem1 = varManager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("分组1"));
    
    QtVariantProperty* item = varManager->addProperty(QVariant::Int, QString("Int: "));
    item->setValue(100);
    //item->setEnabled(false);
    groupItem1->addSubProperty(item);

    QtVariantProperty* item2 = varManager->addProperty(QVariant::Bool, QString("Bool: "));
    item2->setValue(true);
    item->addSubProperty(item2);
    groupItem1->addSubProperty(item2);
    propertyTreeWidget->addProperty(item2);

    item = varManager->addProperty(QVariant::Double, QString("Double: "));
    item->setValue(12.34);
    groupItem1->addSubProperty(item);

    item = varManager->addProperty(QVariant::String, QString("String: "));
    item->setValue(QString("TestTest"));
    groupItem1->addSubProperty(item);
    propertyTreeWidget->addProperty(groupItem1);

    auto* enumFactory = new QtEnumEditorFactory();
    auto* enumManager = new QtEnumPropertyManager();
    propertyTreeWidget->setFactoryForManager(enumManager,enumFactory);
// 初始化，通过enumManager来初始化，给它一个名字“COLOR”
    QtProperty *colorType =  enumManager->addProperty("COLOR");
//这里是将枚举类型装在一个QStringList里
    QStringList colors;
    colors<<"Red"<<"Green"<<"Blue";
//枚举管理器添加枚举值
    enumManager->setEnumNames(colorType,colors);
//添加默认值，如果不写，自动选择0位的值
    enumManager->setValue(colorType,1);
//将这一条值加到browser之中
    propertyTreeWidget->addProperty(colorType);
}

