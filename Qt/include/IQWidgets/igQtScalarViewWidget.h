/**
 * @class   igQtScalarViewWidget
 * @brief   igQtScalarViewWidget's brief
 */

#pragma once
#include <ui_ScalarView.h>
#include <ui_SetCustomScaleRange.h>
#include <QDockWidget>

//namespace iGame {
//	class iGameDataArray;
//	class iGameModelColorManager;
//}
class igQtScalarViewWidget : public QWidget {

	Q_OBJECT

public:
	igQtScalarViewWidget(QWidget* parent = nullptr);


public slots:
	void getScalarsName();
	void showScalarItem();
	void showScalarView();
	void editColorBar();
	void rescaleRange();
	void setCustomScaleRange();
	void showCustomScaleRangeWidget();
	void isShowColorLegend();
	void drawModelWithScalarData();
	void loadScalarData();
	void initScalarRange();
    int getCurrentSelectedScalarIdx();
signals:
	void updateCurrentModelColor();
	void changeColorBarShow();
	void ChangeShowColorManager();
protected:

private:
	Ui::ScalarView* ui;
	//iGame::iGameModelColorManager* modelColorManager;
	QWidget* SetCustomScaleRangeWidget{ nullptr };
	Ui::SetCustomScaleRange* SetCustomScaleRangeUi{ nullptr };
	std::map<std::string, int >scalarInfo;
	int drawItem { -1 };
    int currentSelectedScalarIdx{ -1 };
	std::string scalarName = {"" };
	//iGame::iGameDataArray* scalarData{ nullptr };
	float scalarMin = 0.0, scalarMax = 1.0;
};
