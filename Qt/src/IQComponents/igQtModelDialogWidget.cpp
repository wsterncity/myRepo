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

    propertyTreeWidget = ui->propertyTreeWidget;
    propertyTreeWidget->setHeaderVisible(false);
    propertyManager = new QtVariantPropertyManager(propertyTreeWidget);
    editFactory = new QtVariantEditorFactory(propertyTreeWidget);
    propertyTreeWidget->setFactoryForManager(propertyManager, editFactory);

//<<<<<<< HEAD
    connect(modelTreeWidget, &igQtModelTreeWidget::ChangeCurrentModel, this, &igQtModelDialogWidget::UpdateCurrentModel);


//    QtVariantPropertyManager* varManager = new QtVariantPropertyManager(propertyTreeWidget);
//    QtVariantEditorFactory* editFactory = new QtVariantEditorFactory(propertyTreeWidget);
//    propertyTreeWidget->setFactoryForManager(varManager, editFactory);
//
//    QtProperty* groupItem1 = varManager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("分组1"));
//
//    QtVariantProperty* item = varManager->addProperty(QVariant::Int, QString("Int: "));
//    item->setValue(100);
//    //item->setEnabled(false);
//    groupItem1->addSubProperty(item);
//
//    QtVariantProperty* item2 = varManager->addProperty(QVariant::Bool, QString("Bool: "));
//    item2->setValue(true);
//    item->addSubProperty(item2);
//    groupItem1->addSubProperty(item2);
//    propertyTreeWidget->addProperty(item2);
//
//    item = varManager->addProperty(QVariant::Double, QString("Double: "));
//    item->setValue(12.34);
//    groupItem1->addSubProperty(item);
//
//    item = varManager->addProperty(QVariant::String, QString("String: "));
//    item->setValue(QString("TestTest"));
//    groupItem1->addSubProperty(item);
//    propertyTreeWidget->addProperty(groupItem1);
//
//    auto* enumFactory = new QtEnumEditorFactory();
//    auto* enumManager = new QtEnumPropertyManager();
//    propertyTreeWidget->setFactoryForManager(enumManager,enumFactory);
//// 初始化，通过enumManager来初始化，给它一个名字“COLOR”
//    QtProperty *colorType =  enumManager->addProperty("COLOR");
////这里是将枚举类型装在一个QStringList里
//    QStringList colors;
//    colors<<"Red"<<"Green"<<"Blue";
////枚举管理器添加枚举值
//    enumManager->setEnumNames(colorType,colors);
////添加默认值，如果不写，自动选择0位的值
//    enumManager->setValue(colorType,1);
////将这一条值加到browser之中
//    propertyTreeWidget->addProperty(colorType);
}
#include "Sources/iGameLineTypePointsSource.h"
void igQtModelDialogWidget::UpdateCurrentModel(Model::Pointer model) {
    propertyTreeWidget->clear();
    currentModel = model.get();
//        model->GetDataObject()->
//        model->GetDataObject()->GetMetadata();
    QtVariantPropertyManager* varManager = new QtVariantPropertyManager(propertyTreeWidget);
    QtVariantEditorFactory* editFactory = new QtVariantEditorFactory(propertyTreeWidget);
    propertyTreeWidget->setFactoryForManager(varManager, editFactory);
    QtProperty* properties_groupItem = varManager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("属性"));
    auto metadata = model->GetDataObject()->GetMetadata();
    for(auto& [propName, propValue] : metadata->entries()){
        QtVariantProperty* item = nullptr;
        std::visit([&](auto && arg){
            using T = std::decay_t<decltype(arg)>;  // 获取实际存储的类型
            if constexpr (std::is_same_v<T, int>) {

                item = varManager->addProperty(QVariant::Int, QString(propName.c_str()));
                item->setValue(std::get<int32_t>(propValue));
            } else if constexpr (std::is_same_v<T, uint32_t>){
                item = varManager->addProperty(QVariant::UInt, QString(propName.c_str()));
                item->setValue(std::get<uint32_t>(propValue));
            }

        }, propValue);

        if(item != nullptr){
            properties_groupItem->addSubProperty(item);
        }
    }
    connect(varManager, &QtVariantPropertyManager::valueChanged, this, [&](QtProperty * _t1, const QVariant & _t2){
        auto f = currentModel->GetModelFilter();
        auto filter = reinterpret_cast<LineTypePointsSource*>(f);
        filter->SetResolution(_t2.toInt());
        filter->Execute();
    });
    propertyTreeWidget->addProperty(properties_groupItem);
//=======
    objectGroup = propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("Object propertys"));
    propertyTreeWidget->addProperty(objectGroup);
    
//>>>>>>> 1d5dc5187c47b8a246bf66895c7b590d9076cfae
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