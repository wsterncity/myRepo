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
	connect(iconDelegate, &IconDelegate::iconClicked, [this](const QModelIndex& index) {
		QStandardItem* item = model->itemFromIndex(index);
		if (item) {
			if (itemList[item]) {
				item->setIcon(QIcon(":/Ticon/Icons/EyeballClosed.svg"));
			}
			else {
				item->setIcon(QIcon(":/Ticon/Icons/Eyeball.svg"));
			}
			itemList[item] = !itemList[item];
			printf("%s %d %d\n",item->text().toStdString().c_str(), index.row(), index.column());
			this->currentModelIdx = GetModelIndexFromItem(item);
			auto curObj = iGame::SceneManager::Instance()->GetCurrentScene()->GetModelList()[GetModelIndexFromItem(item)];

			qDebug() << curObj << ' ' << itemList[item];
			curObj->SetVisibility(itemList[item]);
			Q_EMIT UpdateCurrentScene();

			return;
			if (itemModelActors.count(item)) {

				auto curObj = iGame::SceneManager::Instance()->GetCurrentScene()->GetModelList()[GetDrawActorsFromItem(item)];
				curObj->SetVisibility(itemList[item]);
				qDebug() << '1' << GetDrawActorsFromItem(item);
				Q_EMIT UpdateCurrentScene();
				//Q_EMIT ChangeModelVisible(GetDrawActorsFromItem(item), itemList[item]);
			}
			else {
				int childNum = item->rowCount();
				for (int i = 0; i < childNum; i++) {
					auto child = item->child(i, 0);
					if (itemList[item]) child->setIcon(QIcon(":/Ticon/Icons/Eyeball.svg"));
					else child->setIcon(QIcon(":/Ticon/Icons/EyeballClosed.svg"));
					itemList[child] = itemList[item];

					auto curObj = iGame::SceneManager::Instance()->GetCurrentScene()->GetModelList()[GetDrawActorsFromItem(child)];
					curObj->SetVisibility(itemList[item]);
					qDebug() << '2' << GetDrawActorsFromItem(child);
					Q_EMIT UpdateCurrentScene();

					//Q_EMIT ChangeModelVisible(GetDrawActorsFromItem(child), itemList[item]);
				}
			}
		}
		});
	connect(this, &QTreeView::clicked, this, [&](const QModelIndex& index) {
		QStandardItem* item = model->itemFromIndex(index);
		if (item) {
			this->currentModelIdx = GetModelIndexFromItem(item);
			Q_EMIT ChangeCurrentModelIndex(this->currentModelIdx);
		}
		});
	// 连接右键点击事件
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QTreeView::customContextMenuRequested, this, &igQtModelListView::UpdateCustomMenu);

	// sxu's test

	//model->setItem(0, 0, new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "test"));
	//QStandardItem* item1 = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "xs");
	//QStandardItem* item2 = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "rya");
	//QStandardItem* item3 = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "xjj");
	//QStandardItem* item4 = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "ck");
	//QStandardItem* item5 = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), "mky");
	//model->item(0, 0)->setChild(0, 0, item1);
	//model->item(0, 0)->setChild(1, 0, item2);
	//model->item(0, 0)->setChild(2, 0, item3);
	//model->item(0, 0)->setChild(3, 0, item4);
	//model->item(0, 0)->setChild(4, 0, item5);
	//itemList[item1] = true;
	//itemList[item2] = true;
	//itemList[item3] = true;
	//itemList[item4] = true;
	//itemList[item5] = true;
	//itemList[model->item(0, 0)] = true;

	this->contextMenu = new QMenu;
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
	for (auto it = itemModelIds.begin(); it != itemModelIds.end(); ++it) {
		auto item = it.key();
		if (!itemList[item]) {
			item->setIcon(QIcon(":/Ticon/Icons/Eyeball.svg"));
			itemList[item] = true;
			if (itemModelActors.count(item)) {
				Q_EMIT ChangeModelVisible(GetDrawActorsFromItem(item), itemList[item]);
			}
		}
	}
}
void igQtModelListView::HideAllModel()
{
	for (auto it = itemModelIds.begin(); it != itemModelIds.end(); ++it) {
		auto item = it.key();
		if (itemList[item]) {
			item->setIcon(QIcon(":/Ticon/Icons/EyeballClosed.svg"));
			itemList[item] = false;
			if (itemModelActors.count(item)) {
				Q_EMIT ChangeModelVisible(GetDrawActorsFromItem(item), itemList[item]);
			}
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
int igQtModelListView::GetModelIndexFromItem(QStandardItem* item)
{
	if (itemModelIds.count(item)) {
		return this->itemModelIds[item];
	}
	else {
		while (item->parent() != nullptr)item = item->parent();
		return this->itemModelIds[item];
	}
}
int igQtModelListView::GetDrawActorsFromItem(QStandardItem* item)
{
	return this->itemModelActors[item];
}


QStandardItem* igQtModelListView::GetItemFromModelIndex(int currentModelId)
{
	int t = itemModelIds.size();
	for (auto it = itemModelIds.begin(); it != itemModelIds.end(); ++it) {
		if (it.value() == currentModelId)return it.key();
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
	currentModelIdx = nextModelIdx++;
	itemList[newModel] = true;
	itemModelIds[newModel] = currentModelIdx;
}

void igQtModelListView::DeleteCurrentFile() {

	auto item = model->itemFromIndex(this->currentIndex());
	//printf("%s\n", item->text().toStdString());
	if (item && itemModelIds.count(item)) {
		DeleteCurrentModelItem(item);
		itemList.remove(item);
		itemModelIds.remove(item);
		int childNum = item->rowCount();
		for (int i = 0; i < childNum; i++) {
			auto child = item->child(i, 0);
			itemList.remove(item);
			itemModelActors.remove(item);
		}
	}
	item = model->itemFromIndex(this->currentIndex());
	if (item) {
		this->currentModelIdx = GetModelIndexFromItem(item);
		Q_EMIT ChangeCurrentModelIndex(this->currentModelIdx);
	}
	//printf("%s\n", model->itemFromIndex(this->currentIndex())->text().toStdString());
}

void igQtModelListView::ChangeSelected2NextItem() {
	if (itemList.empty()) return;
	currentModelIdx = (currentModelIdx + 1) % itemList.size();
}

void igQtModelListView::ChangeSelected2LastItem() {
	if (itemList.empty()) return;
	currentModelIdx = (currentModelIdx + itemList.size() - 1) % itemList.size();
}

void igQtModelListView::AddChildToItem(QStandardItem* item, const QString& modelName, int actorIndex)
{
	int rowIndex = item->rowCount();
	QStandardItem* newModel = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), modelName);
	item->setChild(rowIndex, 0, newModel);
	this->setCurrentIndex(newModel->index());
	itemList[newModel] = true;
	itemModelActors[newModel] = actorIndex;
}

void igQtModelListView::AddChildToModel(int modelindex, const QString& modelName, int actorIndex)
{
	auto item = GetItemFromModelIndex(modelindex);
	if (item) {
		AddChildToItem(item, modelName, actorIndex);
	}

}
void igQtModelListView::InsertModel(int idx, const QString& modelName) {
	QStandardItem* rootItem = model->invisibleRootItem();
	QStandardItem* newModel = new QStandardItem(QIcon(":/Ticon/Icons/Eyeball.svg"), modelName);
	rootItem->insertRow(idx, newModel);
	this->setCurrentIndex(newModel->index());
	this->currentModelIdx = nextModelIdx++;
	itemList[newModel] = true;
	itemModelIds[newModel] = this->currentModelIdx;
}

igQtModelListView::~igQtModelListView() = default;