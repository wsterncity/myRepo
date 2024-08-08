#include<iGameSceneManager.h>

#include <IQComponents/igQtTreeWidget.h>
#include <QStandardItemModel>
#include <QModelIndexList>
#include <qdebug.h>
#include <QQueue>
#include <qmenu.h>
#include <qaction.h>

igQtTreeWidget::igQtTreeWidget(QWidget* parent) 
	: QDockWidget(parent),
	ui(new Ui::LayerDialog)
{
	ui->setupUi(this);
	modelTreeWidget = ui->modelTreeWidget;


	modelTreeWidget->setColumnCount(2);  // 设置列数
	QStringList headers;
	headers << "Name" << "Properties";
	modelTreeWidget->setHeaderLabels(headers);  // 设置列标题

    // 创建一个项目
    QTreeWidgetItem* item = new QTreeWidgetItem(modelTreeWidget);
    item->setText(0, "bladefem_poly");

    // 设置项目图标
    item->setIcon(0, QIcon(":/Ticon/Icons/Eyeball.svg"));  // 替换为实际图标路径

    // 如果需要，可以添加子项目
    QTreeWidgetItem* sub_item = new QTreeWidgetItem(item);
    sub_item->setText(0, "SubItem");
    sub_item->setText(1, "Properties here");

    // 添加第二列内容
    item->setText(1, "Properties here");

    modelTreeWidget->addTopLevelItem(item);

    // 设置委托
    ButtonDelegate* delegate = new ButtonDelegate(modelTreeWidget);
    modelTreeWidget->setItemDelegateForColumn(1, delegate);
}

