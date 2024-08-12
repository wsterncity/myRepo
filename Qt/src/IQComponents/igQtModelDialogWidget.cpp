#include<iGameSceneManager.h>

#include <IQComponents/igQtModelDialogWidget.h>
#include <qdebug.h>
#include <QQueue>
#include <qmenu.h>
#include <qaction.h>

igQtModelDialogWidget::igQtModelDialogWidget(QWidget* parent)
	: QDockWidget(parent),
	ui(new Ui::LayerDialog)
{
	ui->setupUi(this);
    this->setMinimumWidth(parent->width() / 4);
    
	modelTreeWidget = ui->modelTreeWidget;
	modelTreeWidget->setColumnCount(2);
    modelTreeWidget->header()->hide();
    modelTreeWidget->setColumnWidth(0, 150);

    propertyTreeWidget = ui->propertyTreeWidget;
    propertyTreeWidget->setColumnCount(2);
    propertyTreeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    propertyTreeWidget->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    propertyTreeWidget->setColumnWidth(1, 1);
    propertyTreeWidget->setHeaderLabels(QStringList() << "Property" << "Value");

    ////Global options
    //QTreeWidgetItem* Global = new QTreeWidgetItem(QStringList() << "Global Options");

    //QTreeWidgetItem* FixedFrame = new QTreeWidgetItem(QStringList() << "Fixed Frame");
    //Global->addChild(FixedFrame);

    //propertyTreeWidget->addTopLevelItem(Global);

    QTreeWidgetItem*  global = propertyTreeWidget->addCategory("Global Options");
    propertyTreeWidget->addTextProperty(global, "Editable", "True");
    propertyTreeWidget->addIntegerProperty(global, "Size", 10);
    propertyTreeWidget->addEnumProperty(global, "Color", QStringList{ "Red", "Green", "Blue" }, "Red");




    //// 创建一个项目
    //ModelTreeWidgetItem* item = new ModelTreeWidgetItem(modelTreeWidget);
    //ModelTreeWidgetItem* item2 = new ModelTreeWidgetItem(modelTreeWidget);

    //item->setText(0, "bladefem_poly");
    //item2->setText(0, "bladefem_poly2");

    //modelTreeWidget->addTopLevelItem(item);
    //modelTreeWidget->addTopLevelItem(item2);
    
}

