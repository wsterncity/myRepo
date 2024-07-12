/**
 * @class   igQtTensorWidget
 * @brief   igQtTensorWidget's brief
 */

#pragma once
#include <ui_TensorView.h>

//namespace iGame {
//	class iGameFloatArray;
//	class iGameIntArray;
//	class iGameTensorRepresentation;
//	class iGamePoints;
//	class iGameModelColorManager;
//};

class igQtTensorWidget : public QWidget {

	Q_OBJECT

public:
	igQtTensorWidget(QWidget* parent = nullptr);

public slots:

	void InitTensorAttributes();
	void ShowTensorField();
	void UpdateEllipsoidGlyphData();
	void UpdateEllipsoidGlyphColor();
	void UpdateGlyphScale(double s);
	void UpdateTensorsNameList();
	void UpdateScalarsNameList();
	void InitPointGlyphColors();
	void UpdateComponentsShow(bool);
	void GenerateVectorField();
signals:
	//void DrawEllipsoidGlyph(iGame::iGameFloatArray*, iGame::iGameIntArray*);
	void DrawEllipsoidGlyph();
	void UpdateEllipsoidGlyph();

private:
	Ui::TensorView* ui;
	//iGame::iGamePoints* Points;
	//iGame::iGameFloatArray* TensorAttributes;
	//iGame::iGameTensorRepresentation* tensorManager;
	//iGame::iGameModelColorManager* tensorColorManager;
	//iGame::iGameFloatArray* EllipsoidGlyphPoints;
	//iGame::iGameIntArray* EllipsoidGlyphPointOrders;
	//iGame::iGameFloatArray* EllipsoidGlyphColors;

};
