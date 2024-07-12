#pragma once
#include <QTreeView>
#include <IQCore/igQtExportModule.h>
#include <QString>
#include <QStyledItemDelegate>
#include <QMouseEvent>
#include <QPainter>

class QStandardItemModel;
class QStandardItem;
class QModelIndex;
class QMenu;
class IconDelegate : public QStyledItemDelegate {
	Q_OBJECT
public:
	using QStyledItemDelegate::QStyledItemDelegate;

	bool editorEvent(QEvent* event, QAbstractItemModel* model,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) override {
		if (event->type() == QEvent::MouseButtonPress) {
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			QRect iconRect = option.rect;
			iconRect.setWidth(option.decorationSize.width());
			if (iconRect.contains(mouseEvent->pos())) {
				emit iconClicked(index);
				return true;
			}
		}
		return QStyledItemDelegate::editorEvent(event, model, option, index);
	}
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
		QStyledItemDelegate::paint(painter, option, index);

		const QTreeView* treeView = qobject_cast<const QTreeView*>(option.widget);
		if (!treeView) {
			return;
		}
		QModelIndex parentIndex = index.parent();
		if (parentIndex.isValid()) {
			QRect parentRect = treeView->visualRect(parentIndex);
			int lineX1 = parentRect.left();
			int lineY1 = parentRect.bottom();
			for (int i = 0; i < treeView->model()->rowCount(parentIndex); ++i) {
				QModelIndex childIndex = parentIndex.child(i, 0);
				QRect currentRect = treeView->visualRect(childIndex);
				int lineX2 = currentRect.left();
				int lineY2 = currentRect.center().y();
				QPen pen(Qt::black, 1);
				painter->setPen(pen);
				painter->drawLine(lineX1, lineY1, lineX1, lineY2);
				painter->drawLine(lineX1, lineY2, lineX2, lineY2);
			}
		}
	}

signals:
	void iconClicked(const QModelIndex& index);
};

class IG_QT_MODULE_EXPORT igQtModelListView : public QTreeView {
	Q_OBJECT

public:
	igQtModelListView(QWidget* parent);

	~igQtModelListView() override;
public:
	int GetModelIndexFromItem(QStandardItem* item);
	int GetDrawActorsFromItem(QStandardItem* item);

	QStandardItem* GetItemFromModelIndex(int currentModelIndex);
	void DeleteCurrentModelItem(QStandardItem* item);
public slots:
	void AddModel(QString modelName);

	void InsertModel(int idx, const QString& modelName);

	void DeleteCurrentFile();

	void ChangeSelected2NextItem();

	void ChangeSelected2LastItem();

	void AddChildToItem(QStandardItem* item, const QString& fileName, int actorIndex);

	void AddChildToModel(int modelindex, const QString& fileName, int actorIndex);

	void UpdateCustomMenu(const QPoint& point);
	void ShowAllModel();
	void HideAllModel();
signals:
	void ChangeModelVisible(int modelIdx, bool visible);
	void ChangeCurrentModelIndex(int modelIdx);
	void OpenFile();
	void DeleteModel();

	void UpdateCurrentScene();
protected:
	int currentModelIdx{ -1 };
	int nextModelIdx{ 0 };
	QStandardItemModel* model;
	//QList<bool> itemList;
	QMap<QStandardItem*, bool>itemList;
	QMap<QStandardItem*, int>itemModelIds;
	QMap<QStandardItem*, int>itemModelActors;
	IconDelegate* iconDelegate;
	QMenu* contextMenu;
protected:

};