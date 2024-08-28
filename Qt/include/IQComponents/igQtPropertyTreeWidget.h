#pragma once

#include <iGameModel.h>

#include <IQCore/igQtExportModule.h>

#include <QMouseEvent>
#include <QDockWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QLineEdit>
#include <qboxlayout.h>
#include <QLabel.h>
#include <QStyledItemDelegate>
#include <QPainter>
#include <iostream>
#include <Plugin/qtpropertybrowser/qteditorfactory.h>
#include <Plugin/qtpropertybrowser/qttreepropertybrowser.h>
#include <Plugin/qtpropertybrowser/qtvariantproperty.h>

class PropertyLineEdit : public QLineEdit {
    Q_OBJECT

public:
    PropertyLineEdit(QWidget* parent = nullptr) : QLineEdit(parent) {}

    template<typename Functor, typename ...Args>
    void setFocusInFunctor(Functor&& functor, Args&&... args) {
        focusInFunctor = std::bind(functor, args...);
    }

    template<typename Functor, typename ...Args>
    void setFocusOutFunctor(Functor&& functor, Args&&... args) {
        focusOutFunctor = std::bind(functor, args...);
    }

protected:
    void focusInEvent(QFocusEvent* event) override {
        QLineEdit::focusInEvent(event);
        if (focusInFunctor) {
            focusInFunctor();
        }
    }
    void focusOutEvent(QFocusEvent* event) override {
        QLineEdit::focusOutEvent(event);
        if (focusOutFunctor) {
            focusOutFunctor();
        }
    }

private:
    std::function<void()> focusInFunctor;
    std::function<void()> focusOutFunctor;
};

class IG_QT_MODULE_EXPORT PropertyTreeWidgetItem : public QTreeWidgetItem {
public:
    enum ValueType {
        CheckBox,
        LineEdit,
        SpinBox,
        ComboBox
    };

    PropertyTreeWidgetItem(QTreeWidgetItem* parent)
        : QTreeWidgetItem(parent)
    {
        
    }

    void setNameText(const QString& text) {
        this->name = text;
        setText(0, name);
    }

    void setValueWidget(QWidget* widget, ValueType type) {
        this->value = widget;
        this->valueType = type;
    }

    void setValueText(const QString& text) {
        this->plainText = text;
        setText(1, text);
    }

    void showWidget() {
        value->show();
    }

    void hideWidget(QTreeWidget* treeWidget) {
        treeWidget->removeItemWidget(this, 1);
        setText(1, plainText);
    }

    QWidget* getWidget() const {
        return value;
    }


private:
    QString name;
    ValueType valueType;
    QWidget* value = nullptr;
    QString plainText;
    bool selected = false;
    bool focus = false;
    friend class igQtPropertyTreeWidget;
};


class EditTextWidget : public QWidget {
    Q_OBJECT

public:
    EditTextWidget(QWidget* parent = nullptr)
        : QWidget(parent), isEditing(false) {
        textLabel = new QLabel(this);
        lineEdit = new PropertyLineEdit(this);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);  // 移除布局的外边距
        layout->addWidget(textLabel);
        layout->addWidget(lineEdit);

        lineEdit->hide(); // 初始状态下隐藏lineEdit
        textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        lineEdit->setFocusOutFunctor(&EditTextWidget::finishEditing, this);
    }

    void setText(const QString& text) {
        textLabel->setText(text);
        lineEdit->setText(text);
    }

    QString text() const {
        return lineEdit->text();
    }

    void startEditing() {
        textLabel->hide();
        lineEdit->show();
        lineEdit->setFocus();
        isEditing = true;

    }

    void finishEditing() {
        if (isEditing) {
            textLabel->setText(lineEdit->text());
            lineEdit->hide();
            textLabel->show();
            isEditing = false;
        }
    }


protected:
    void resizeEvent(QResizeEvent* event) override {
        QWidget::resizeEvent(event);
        textLabel->setFixedHeight(event->size().height());
        lineEdit->setFixedHeight(event->size().height());
    }

private:
    QLabel* textLabel;
    PropertyLineEdit* lineEdit;
    bool isEditing;
};


class CustomDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    CustomDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        // 默认绘制
        QStyledItemDelegate::paint(painter, option, index);

        // 计算项的矩形
        QRect rect = option.rect;

        // 去掉缩进部分的边框
        int indent = 20; // 假设缩进为 20 像素
        QRect contentRect = rect.adjusted(indent, 0, 0, 0);

        // 绘制边框（去掉缩进部分的边框）
        painter->save();
        painter->setPen(QPen(Qt::black));
        painter->drawRect(contentRect.adjusted(0, 0, -1, -1)); // 绘制内容区域边框
        painter->restore();
    }
};


class IG_QT_MODULE_EXPORT igQtPropertyTreeWidget : public QTreeWidget {
    Q_OBJECT

public:
    igQtPropertyTreeWidget(QWidget* parent = nullptr);

    // 添加属性分类
    QTreeWidgetItem* addCategory(const QString& name);

    // 添加属性项 (字符串类型)
    void addTextProperty(QTreeWidgetItem* category, const QString& name, const QString& defaultValue = "");

    // 添加属性项 (整数类型)
    void addIntegerProperty(QTreeWidgetItem* category, const QString& name, int defaultValue = 0);

    // 添加属性项 (枚举类型)
    void addEnumProperty(QTreeWidgetItem* category, const QString& name, const QStringList& options, const QString& defaultValue = "");

    void onItemClicked(QTreeWidgetItem* item, int column) {
        if (column == 1) {
            QWidget* widget = itemWidget(item, 1);
            EditTextWidget* editTextWidget = dynamic_cast<EditTextWidget*>(widget);
            if (editTextWidget) {
                editTextWidget->startEditing();
            }
        }
    }

};