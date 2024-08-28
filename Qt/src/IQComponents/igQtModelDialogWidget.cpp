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

    //QSplitter* splitter = new QSplitter(Qt::Vertical, this);
    //this->setWidget(splitter);
    //splitter->addWidget(ui->modelTreeWidget);
    //splitter->addWidget(ui->propertyTreeWidget);
    //splitter->setChildrenCollapsible(false);

	modelTreeWidget = ui->modelTreeWidget;
    propertyTreeWidget = ui->propertyTreeWidget;

    modelTreeWidget->setColumnCount(2);
    modelTreeWidget->header()->hide();
    modelTreeWidget->setColumnWidth(0, 150);

    propertyTreeWidget = ui->propertyTreeWidget;
    propertyTreeWidget->setHeaderVisible(false);
    propertyManager = new QtVariantPropertyManager(propertyTreeWidget);
    editFactory = new QtVariantEditorFactory(propertyTreeWidget);
    propertyTreeWidget->setFactoryForManager(propertyManager, editFactory);

    objectGroup = propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("Object propertys"));
    propertyTreeWidget->addProperty(objectGroup);
    
	ui->pushButton;
}

int igQtModelDialogWidget::addDataObjectToModelTree(DataObject::Pointer obj, ItemSource source) {
	// 创建一个项目
	ModelTreeWidgetItem* item = new ModelTreeWidgetItem(modelTreeWidget);
	auto scene = iGame::SceneManager::Instance()->GetCurrentScene();
	auto model = scene->CreateModel(obj);
	int id = scene->AddModel(model);

	item->setName(QString::fromStdString(obj->GetName()));
	item->setModel(model);

	auto attrSet = obj->GetAttributeSet()->GetAllAttributes();
	for (int i = 0; i < attrSet->GetNumberOfElements(); i++) {
		auto& attr = attrSet->GetElement(i);
		if (attr.isDeleted) continue;
		QTreeWidgetItem* child = new QTreeWidgetItem(item);
		child->setText(0, QString::fromStdString(attr.pointer->GetName()));
		child->setIcon(0, QIcon(":/Ticon/Icons/select/file.png"));
		child->setData(0, Qt::UserRole, i);
		//int index = child->data(0, Qt::UserRole).toInt();
		//std::cout << index << std::endl;
	}


	modelTreeWidget->addTopLevelItem(item);
	modelTreeWidget->setCurrentItem(item);

	//propertyTreeWidget->removeProperty(objectGroup);
	//objectGroup = propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("Object propertys"));
	//propertyTreeWidget->addProperty(objectGroup);
	//auto* props = obj->GetPropertys();
	//for (int i = 0; i < props->Size(); i++) {
	//	auto prop = props->GetProperty(i);
	//	QtVariantProperty* item = propertyManager->addProperty(QVariant::Int, QString::fromStdString(prop->GetName()));
	//	item->setValue(prop->Get<int>());
	//	item->setEnabled(prop->IsEnabled());
	//	objectGroup->addSubProperty(item);

	//	for (int j = 0; j < prop->Size(); j++) {
	//		auto subProp = prop->GetSubProperty(j);
	//		QtVariantProperty* subItem = propertyManager->addProperty(QVariant::Int, QString::fromStdString(subProp->GetName()));
	//		subItem->setValue(subProp->Get<int>());
	//		subItem->setEnabled(prop->IsEnabled());
	//		item->addSubProperty(subItem);
	//	}
	//}
	return id;
}

int igQtModelDialogWidget::addModelToModelTree(Model::Pointer model) {
	ModelTreeWidgetItem* item = new ModelTreeWidgetItem(modelTreeWidget);
	auto scene = iGame::SceneManager::Instance()->GetCurrentScene();
	int id = scene->AddModel(model);

	item->setName(QString::fromStdString(model->GetDataObject()->GetName()));
	item->setModel(model);
	modelTreeWidget->addTopLevelItem(item);
	modelTreeWidget->setCurrentItem(item);
	return id;
}