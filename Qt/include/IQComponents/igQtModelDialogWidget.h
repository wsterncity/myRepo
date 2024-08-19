#pragma once

#include <iGameSceneManager.h>

#include <IQComponents/igQtModelTreeWidget.h>
#include <IQComponents/igQtPropertyTreeWidget.h>
#include <IQCore/igQtExportModule.h>

#include <ui_layerDialog.h>

#include <QString>
#include <QMouseEvent>
#include <QDockWidget>
#include <QTreeWidget>
#include <iostream>
#include <Plugin/qtpropertybrowser/qteditorfactory.h>
#include <Plugin/qtpropertybrowser/qttreepropertybrowser.h>
#include <Plugin/qtpropertybrowser/qtvariantproperty.h>

using namespace iGame;

class IG_QT_MODULE_EXPORT igQtModelDialogWidget : public QDockWidget {
	Q_OBJECT
public:
	igQtModelDialogWidget(QWidget* parent);
	~igQtModelDialogWidget() override = default;

public slots:
	void addDataObjectToModelTree(DataObject::Pointer obj, ItemSource source);

private:
	igQtModelTreeWidget* modelTreeWidget;
	QtTreePropertyBrowser* propertyTreeWidget;
	
	QtVariantPropertyManager* propertyManager;
	QtVariantEditorFactory* editFactory;
	QtProperty* objectGroup;
	Ui::LayerDialog* ui;
};

