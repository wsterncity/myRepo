#pragma once

#include <iGameSceneManager.h>

#include <ui_layerDialog.h>

#include <IQComponents/igQtModelTreeWidget.h>
#include <IQCore/igQtExportModule.h>
#include <QString>
#include <QMouseEvent>
#include <QDockWidget>
#include <QTreeWidget>
#include <iostream>

class IG_QT_MODULE_EXPORT igQtTreeWidget : public QDockWidget {
	Q_OBJECT
public:
	igQtTreeWidget(QWidget* parent);
	~igQtTreeWidget() override = default;

public slots:
	void add() {
		// 创建一个项目
		ModelTreeWidgetItem* item = new ModelTreeWidgetItem(modelTreeWidget);

		item->setText(0, "bladefem_poly");
		item->setModel(iGame::SceneManager::Instance()->GetCurrentScene()->GetCurrentModel());
		modelTreeWidget->addTopLevelItem(item);
	}

private:
	igQtModelTreeWidget* modelTreeWidget;
	Ui::LayerDialog* ui;
};

