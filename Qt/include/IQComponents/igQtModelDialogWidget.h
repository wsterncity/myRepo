#pragma once

#include <iGameSceneManager.h>

#include <IQComponents/igQtModelTreeWidget.h>
#include <IQCore/igQtExportModule.h>

#include <ui_layerDialog.h>

#include <QString>
#include <QMouseEvent>
#include <QDockWidget>
#include <QTreeWidget>
#include <iostream>

using namespace iGame;

class IG_QT_MODULE_EXPORT igQtModelDialogWidget : public QDockWidget {
	Q_OBJECT
public:
	igQtModelDialogWidget(QWidget* parent);
	~igQtModelDialogWidget() override = default;

public slots:
	void addDataObjectToModelTree(DataObject::Pointer obj, ItemSource source) {
		// 创建一个项目
		ModelTreeWidgetItem* item = new ModelTreeWidgetItem(modelTreeWidget);
		auto scene = iGame::SceneManager::Instance()->GetCurrentScene();
		auto model = scene->CreateModel(obj);
		int id = scene->AddModel(model);

		item->setName(QString::fromStdString(obj->GetName()));
		item->setModel(model);
		
		//QTreeWidgetItem* child = new QTreeWidgetItem(item);
		//child->setText(0, "Source");
		//child->setText(1, "File");
		modelTreeWidget->addTopLevelItem(item);
		modelTreeWidget->setCurrentItem(item);
	}

private:
	igQtModelTreeWidget* modelTreeWidget;
	Ui::LayerDialog* ui;
};

