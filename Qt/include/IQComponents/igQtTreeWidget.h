#pragma once

#include <ui_layerDialog.h>

#include <QTreeView>
#include <IQCore/igQtExportModule.h>
#include <QString>
#include <QStyledItemDelegate>
#include <QMouseEvent>
#include <QPainter>
#include <QDockWidget>
#include <QTreeWidget>
#include <QPushButton>

class ButtonDelegate : public QStyledItemDelegate {
public:
    ButtonDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (index.column() == 1) {  // 只在第二列添加按钮
            QWidget* editor = new QWidget(parent);
            QHBoxLayout* layout = new QHBoxLayout(editor);
            layout->setContentsMargins(0, 0, 0, 0);
            QPushButton* button = new QPushButton(editor);
            button->setIcon(QIcon(":/Ticon/Icons/Eyeball.svg"));  // 替换为实际按钮图标路径
            layout->addWidget(button);
            return editor;
        }
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
};

class IG_QT_MODULE_EXPORT igQtTreeWidget : public QDockWidget {
	Q_OBJECT
public:
	igQtTreeWidget(QWidget* parent);
	~igQtTreeWidget() override = default;



private:
	QTreeWidget* modelTreeWidget;
	Ui::LayerDialog* ui;
};

