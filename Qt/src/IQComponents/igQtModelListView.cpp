//#include "igQtModelListView.h"
#include<iGameSceneManager.h>

#include <IQComponents/igQtModelListView.h>
#include <QStandardItemModel>
#include <QModelIndexList>
#include <qdebug.h>
#include <QQueue>
#include <qmenu.h>
#include <qaction.h>

igQtModelListView::igQtModelListView(QWidget* parent) : QTreeView(parent) {
	this->setHeaderHidden(true);
	// Create a QStandardItemModel to hold data
	model = new QStandardItemModel(this);
	// Populate the model with some data
	this->setModel(model);
	this->setStyleSheet("QTreeView::item:selected { background-color:  rgb(0, 115, 153); color: white; }");

	iconDelegate = new IconDelegate(this);
	setItemDelegate(iconDelegate);
	//setItemDelegate(new LineDelegate(this));
	connect(iconDelegate, &IconDelegate::iconClicked, [&](const QModelIndex& index) {
		QStandardItem* item = model->itemFromIndex(index);
		if (item) {
			ReverseItemVisibility(item);
			//printf("%s %d %d\n",item->text().toStdString().c_str(), index.row(), index.column());
			int id = GetObjectIdFromItem(item);
			auto curObj = iGame::SceneManager::Instance()->GetCurrentScene()->GetDataObject(GetObjectIdFromItem(item));

			//qDebug() << curObj << ' ' << itemVisibleList[item];
			curObj->SetVisibility(itemVisibleList[item]);
			Q_EMIT UpdateCurrentScene();

			for(int i = 0; i < item->rowCount(); i ++){
				auto child = item->child(i);
				qDebug() << child->row();
				if(itemVisibleList[child] != itemVisibleList[item]) ReverseItemVisibility(child);
			}

			auto par = item->parent();
			if(par != nullptr){
				bool haveVisibleChild = false;
				for(int i = 0; i < par->rowCount(); i ++){
					if(itemVisibleList[par->child(i)]){
						haveVisibleChild = true;
						break;
					}
				}
				if((itemVisibleList[par] && !haveVisibleChild) || (!itemVisibleList[par] && haveVisibleChild) ) ReverseItemVisibility(par);
			}


//			if(par == nullptr) rootItem->insertRow(item->row() + 1, newModel);

//			return;
//			if (itemModelActors.count(item)) {
//
//				auto curObj = iGame::SceneManager::Instance()->GetCurrentScene()->GetModelList()[GetDrawActorsFromItem(item)];
//				curObj->SetVisibility(itemVisibleList[item]);
//				qDebug() << '1' << GetDrawActorsFromItem(item);
//				Q_EMIT UpdateCurrentScene();
//				//Q_EMIT ChangeModelVisible(GetDrawActorsFromItem(item), itemVisibleList[item]);
//			}
//			else {
//				int childNum = item->rowCount();
//				for (int i = 0; i < childNum; i++) {
//					auto child = item->child(i, 0);
//					if (itemVisibleList[item]) child->setIcon(QIcon(":/Ticon/Icons/Eyeball.svg"));
//					else child->setIcon(QIcon(":/Ticon/Icons/EyeballClosed.svg"));
//					itemVisibleList[child] = itemVisibleList[item];
//
//					auto curObj = iGame::SceneManager::Instance()->GetCurrentScene()->GetModelList()[GetDrawActorsFromItem(child)];
//					curObj->SetVisibility(itemVisibleList[item]);
//					qDebug() << '2' << GetDrawActorsFromItem(child);
//					Q_EMIT UpdateCurrentScene();
//
//					//Q_EMIT ChangeModelVisible(GetDrawActorsFromItem(child), itemVisibleList[item]);
//				}
//			}
		}
		});
	connect(this, &QTreeView::clicked, this, [&](const QModelIndex& index) {
		QStandardItem* item = model->itemFromIndex(index);
		this->setCurrentIndex(index);
		if (item) {
			this->currentObjectIdx = GetObjectIdFromItem(item);
//			qDebug() << currentObjectIdx;
			iGame::SceneManager::Instance()->GetCurrentScene()->UpdateCurrentDataObject(currentObjectIdx);
		}
		});

	// 连接右键点击事件
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QTreeView::customContextMenuRequested, this, &igQtModelListView::UpdateCustomMenu);

	// sxu's test

//	model->setItem(0, 0, new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "test"));
//	QStandardItem* item1 = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "xs");
//	QStandardItem* item2 = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "rya");
//	QStandardItem* item3 = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "xjj");
//	QStandardItem* item4 = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "ck");
//	QStandardItem* item5 = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "mky");
//	model->item(0, 0)->setChild(0, 0, item1);
//	model->item(0, 0)->setChild(1, 0, item2);
//	model->item(0, 0)->setChild(2, 0, item3);
//	model->item(0, 0)->setChild(3, 0, item4);
//	model->item(0, 0)->setChild(4, 0, item5);
//	itemVisibleList[item1] = true;
//	itemVisibleList[item2] = true;
//	itemVisibleList[item3] = true;
//	itemVisibleList[item4] = true;
//	itemVisibleList[item5] = true;
//	itemVisibleList[model->item(0, 0)] = true;

//    AddModel("model0");
//    AddModel("model1");
//    AddModel("model2");
//    InsertObject(3, "aaa");
//    AddChildToModel(2, "child");
//    InsertObject(3, "bbb");


	this->contextMenu = new QMenu(this);
	QAction* action_Open = new QAction(QIcon(":/Ticon/Icons/icon (5).png"), "Open", this->contextMenu);
	QAction* action_ShwoAll = new QAction(QIcon(":/Ticon/Icons/Eyeball.svg"), "Show All", this->contextMenu);
	QAction* action_HideAll = new QAction(QIcon(":/Ticon/Icons/EyeballClosed.svg"), "Hide All", this->contextMenu);
	QAction* action_Delete = new QAction(QIcon(":/Ticon/Icons/Delete.png"), "Delete", this->contextMenu);
	QAction* action_Rename = new QAction("Rename", this->contextMenu);
	QAction* action_Reload = new QAction("Reload", this->contextMenu);

	QAction* action_Extract = new QAction("Extract Group", this->contextMenu);
	QAction* action_AddPlane = new QAction("Add Plane", this->contextMenu);

	connect(action_Open, &QAction::triggered, this, [&]() {Q_EMIT OpenFile(); });
	connect(action_ShwoAll, &QAction::triggered, this, &igQtModelListView::ShowAllModel);
	connect(action_HideAll, &QAction::triggered, this, &igQtModelListView::HideAllModel);
	connect(action_Delete, &QAction::triggered, this, [&]() {Q_EMIT DeleteModel(); });
	connect(action_Rename, &QAction::triggered, this, [&]() {edit(currentIndex()); });

	contextMenu->addAction(action_Open);
	contextMenu->addAction(action_ShwoAll);
	contextMenu->addAction(action_HideAll);
	contextMenu->addSeparator();
	contextMenu->addAction(action_Delete);
	contextMenu->addAction(action_Rename);
	contextMenu->addAction(action_Reload);
	contextMenu->addSeparator();
	contextMenu->addAction(action_Extract);
	contextMenu->addAction(action_AddPlane);
}
void igQtModelListView::ShowAllModel()
{
	for (auto it = itemObjectIds.begin(); it != itemObjectIds.end(); ++it) {
		auto item = it.key();
		if (!itemVisibleList[item]) {
			item->setIcon(QIcon(":/Ticon/Icons/Eyeball.svg"));
			itemVisibleList[item] = true;
//			if (itemModelActors.count(item)) {
//				Q_EMIT ChangeModelVisible(GetDrawActorsFromItem(item), itemVisibleList[item]);
//			}
		}
	}
}
void igQtModelListView::HideAllModel()
{
	for (auto it = itemObjectIds.begin(); it != itemObjectIds.end(); ++it) {
		auto item = it.key();
		if (itemVisibleList[item]) {
			item->setIcon(QIcon(":/Ticon/Icons/EyeballClosed.svg"));
			itemVisibleList[item] = false;
//			if (itemModelActors.count(item)) {
//				Q_EMIT ChangeModelVisible(GetDrawActorsFromItem(item), itemVisibleList[item]);
//			}
		}
	}
}
void igQtModelListView::UpdateCustomMenu(const QPoint& point)
{
	QModelIndex index = indexAt(point);
	if (index.isValid()) {
		QStandardItem* item = model->itemFromIndex(index);
		if (item) {
			this->contextMenu->exec(mapToGlobal(point));
		}
	}
	else {
		this->contextMenu->exec(mapToGlobal(point));
	}
}
int igQtModelListView::GetObjectIdFromItem(QStandardItem *item) {
    return itemObjectIds.count(item) ? itemObjectIds[item] : -1;
}
//int igQtModelListView::GetDrawActorsFromItem(QStandardItem* item)
//{
//	return this->itemModelActors[item];
//}


QStandardItem* igQtModelListView::GetObjectItemFromObjectId(int currentObjectId)
{
	for (auto it = itemObjectIds.begin(); it != itemObjectIds.end(); ++it) {
		if (it.value() == currentObjectId)return it.key();
	}
	return nullptr;
}
void igQtModelListView::DeleteCurrentModelItem(QStandardItem* deleteItem)
{
	QQueue<QStandardItem*>Q;
	for (int i = 0; i < model->rowCount(); i++) {
		if (model->item(i, 0) == deleteItem) {
			model->removeRow(i);
			return;
		}
		Q.enqueue(model->item(i, 0));
	}
	while (!Q.empty()) {
		auto item = Q.first();
		for (int i = 0; i < item->rowCount(); i++) {
			if (item->child(i) == deleteItem) {
				item->removeRow(i);
				return;
			}
			Q.push_back(item->child(i));
		}
		Q.dequeue();
	}
}
void igQtModelListView::AddModel(QString modelName) {
	QStandardItem* rootItem = model->invisibleRootItem();
	QStandardItem* newModel = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), modelName);
	rootItem->appendRow(newModel);
	this->setCurrentIndex(newModel->index());
	currentObjectIdx = nextObjectIdx++;
	itemVisibleList[newModel] = true;
	itemObjectIds[newModel] = currentObjectIdx;

	auto curObj = iGame::SceneManager::Instance()->GetCurrentScene()->GetCurrentObject();
	if(curObj->HasSubDataObject()){
		for(auto it = curObj->SubBegin(); it != curObj->SubEnd(); it ++){

			auto subObj = it->second;
			AddChildToItem(newModel, "blk_0");
		}
	}
}

void igQtModelListView::DeleteCurrentFile() {

	auto item = model->itemFromIndex(this->currentIndex());
	//printf("%s\n", item->text().toStdString());
	if (item && itemObjectIds.count(item)) {
		DeleteCurrentModelItem(item);
		itemVisibleList.remove(item);
		itemObjectIds.remove(item);
		int childNum = item->rowCount();
		for (int i = 0; i < childNum; i++) {
			auto child = item->child(i, 0);
			itemVisibleList.remove(child);
            itemObjectIds.remove(child);
//			itemModelActors.remove(child);
		}
	}
	item = model->itemFromIndex(this->currentIndex());
	if (item) {
//		this->currentModelIdx = GetModelIndexFromItem(item);
        this->currentObjectIdx = GetObjectIdFromItem(item);
//		Q_EMIT ChangeCurrentModelIndex(this->currentModelIdx);
		Q_EMIT ChangeCurrentModelIndex(this->currentObjectIdx);
	}
	//printf("%s\n", model->itemFromIndex(this->currentIndex())->text().toStdString());
}

void igQtModelListView::ChangeSelected2NextItem() {
	if (itemVisibleList.empty()) return;
	currentObjectIdx = (currentObjectIdx + 1) % itemVisibleList.size();
}

void igQtModelListView::ChangeSelected2LastItem() {
	if (itemVisibleList.empty()) return;
    currentObjectIdx = (currentObjectIdx + itemVisibleList.size() - 1) % itemVisibleList.size();
}

void igQtModelListView::AddChildToItem(QStandardItem* parentItem, const QString& modelName)
{
    int rowIndex = parentItem->rowCount();
    QStandardItem* newModel = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), modelName);
    parentItem->setChild(rowIndex, 0, newModel);
    this->setCurrentIndex(newModel->index());
    itemVisibleList[newModel] = true;
    currentObjectIdx = nextObjectIdx ++;
    itemObjectIds[newModel] = currentObjectIdx;
//    itemModelActors[newModel] = actorIndex;
}

void igQtModelListView::AddChildToModel(int modelindex, const QString& modelName)
{
    auto item = GetObjectItemFromObjectId(modelindex);
    if (item) {
        AddChildToItem(item, modelName);
    }

}
void igQtModelListView::InsertObject(int idx, const QString& modelName) {
	QStandardItem* rootItem = model->invisibleRootItem();
    QStandardItem* item = nullptr;
    while(item == nullptr)item = GetObjectItemFromObjectId(idx --);

	QStandardItem* newModel = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), modelName);

//    item->appendRow(newModel);
    auto par = item->parent();
    if(par == nullptr) rootItem->insertRow(item->row() + 1, newModel);
    else par->setChild(item->row() + 1, newModel);
//    par->setChild(item->row() + 1, 0, newModel);
//            item->insertRow(idx, newModel);
    this->setCurrentIndex(newModel->index());
    this->currentObjectIdx = nextObjectIdx++;
	itemVisibleList[newModel] = true;
	itemObjectIds[newModel] = this->currentObjectIdx;
}

void igQtModelListView::ReverseItemVisibility(QStandardItem *item) {
	if (itemVisibleList[item]) {
		item->setIcon(QIcon(":/Ticon/Icons/EyeballClosed.svg"));
	} else {
		item->setIcon(QIcon(":/Ticon/Icons/Eyeball.svg"));
	}
	itemVisibleList[item] = !itemVisibleList[item];
}

igQtModelListView::~igQtModelListView() = default;