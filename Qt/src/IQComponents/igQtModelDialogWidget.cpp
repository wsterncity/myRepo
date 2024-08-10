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
	modelTreeWidget->setColumnCount(2);  // 设置列数
    modelTreeWidget->header()->hide();
    modelTreeWidget->setColumnWidth(0, 150);


    //// 创建一个项目
    //ModelTreeWidgetItem* item = new ModelTreeWidgetItem(modelTreeWidget);
    //ModelTreeWidgetItem* item2 = new ModelTreeWidgetItem(modelTreeWidget);

    //item->setText(0, "bladefem_poly");
    //item2->setText(0, "bladefem_poly2");

    //modelTreeWidget->addTopLevelItem(item);
    //modelTreeWidget->addTopLevelItem(item2);
    
}

