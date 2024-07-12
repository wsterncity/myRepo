#include<IQWidgets/igQtTensorWidget.h>
//#include "iGameTensorRepresentation.h"
//#include <iGameModelColorManager.h>
//#include <time.h>
//#include "iGameManager.h"
/**
 * @class   igQtTensorWidget
 * @brief   igQtTensorWidget's brief
 */
igQtTensorWidget::igQtTensorWidget(QWidget* parent) : QWidget(parent), ui(new Ui::TensorView) {
	ui->setupUi(this);
	UpdateComponentsShow(false);
	//this->tensorManager = iGame::iGameTensorRepresentation::New();
	//this->tensorManager->SetSliceNum(10);
	//this->tensorColorManager = iGame::iGameModelColorManager::Instance();
	//this->EllipsoidGlyphPoints = iGame::iGameFloatArray::New();
	//this->EllipsoidGlyphPointOrders = iGame::iGameIntArray::New();
	//this->EllipsoidGlyphColors = iGame::iGameFloatArray::New();


	//connect(ui->TensorInfoComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(InitTensorAttributes()));
	//connect(ui->btn_Apply, SIGNAL(clicked(bool)), this, SLOT(ShowTensorField()));
	//QRegExp rx = QRegExp("^?\\d+(\\.\\d+)?([eE][-+]?\\d+)?$");
	//ui->lineEdit_TensorScale->setValidator(new QRegExpValidator(rx, this));
	//connect(ui->horizontalSlider_ScaleSlider, &QSlider::valueChanged, this, [&]() {
	//	double scale = ui->horizontalSlider_ScaleSlider->value() * 1.0 / 1000.0;
	//	this->UpdateGlyphScale(scale);
	//	std::stringstream ss;
	//	ss << std::setprecision(4) << scale;
	//	ui->lineEdit_TensorScale->setText(QString::fromStdString(ss.str()));
	//	});
	//connect(ui->lineEdit_TensorScale, &QLineEdit::editingFinished, this, [&]() {
	//	QString data = ui->lineEdit_TensorScale->text();
	//	std::stringstream ss(data.toStdString());
	//	double scale = .0;
	//	ss >> scale;
	//	this->UpdateGlyphScale(scale);
	//	ui->horizontalSlider_ScaleSlider->setValue(scale * 1000.0);
	//	});
	//connect(ui->ScalarInfoComboBox, &QComboBox::currentTextChanged, this, [&]() {
	//	this->UpdateEllipsoidGlyphColor();
	//	Q_EMIT UpdateEllipsoidGlyph();
	//	});
	//connect(ui->btn_GenerateVector, SIGNAL(clicked(bool)), this, SLOT(GenerateVectorField()));


}
void igQtTensorWidget::UpdateComponentsShow(bool flag)
{
	if (flag) {
		ui->lineEdit_TensorScale->show();
		ui->horizontalSlider_ScaleSlider->show();
		ui->ScalarInfoComboBox->show();
		ui->label_ColorFrom->show();
		ui->label_ScaleData->show();
		ui->btn_Apply->hide();
	}
	else {
		ui->lineEdit_TensorScale->hide();
		ui->horizontalSlider_ScaleSlider->hide();
		ui->ScalarInfoComboBox->hide();
		ui->label_ColorFrom->hide();
		ui->label_ScaleData->hide();
		ui->btn_Apply->show();
	}
}
void igQtTensorWidget::UpdateScalarsNameList()
{
	//ui->ScalarInfoComboBox->clear();
	//ui->ScalarInfoComboBox->addItem("Solider");
	//auto manager = iGame::iGameManager::Instance();
	//if (manager->GetModelList().size() == 0)return;
	//auto model = manager->GetCurrentModel();
	//auto dataset = model->DataSet;
	//if (dataset->GetPointData() == nullptr)return;
	//auto attributes = dataset->GetPointData()->GetAllScalars();
	//for (int i = 0; i < attributes.size(); i++) {
	//	auto data = dataset->GetPointData()->GetScalars(i);
	//	std::string name = data->GetName();
	//	ui->ScalarInfoComboBox->addItem(QString::fromStdString(name));
	//}
}
void igQtTensorWidget::UpdateTensorsNameList()
{
	//ui->TensorInfoComboBox->clear();
	//auto manager = iGame::iGameManager::Instance();
	//if (manager->GetModelList().size() == 0)return;
	//auto model = manager->GetCurrentModel();
	//auto dataset = model->DataSet;
	//if (dataset->GetPointData() == nullptr)return;
	//auto attributes = dataset->GetPointData()->GetAllTensors();
	//for (int i = 0; i < attributes.size(); i++) {
	//	auto data = dataset->GetPointData()->GetTensors(i);
	//	std::string name = data->GetName();
	//	ui->TensorInfoComboBox->addItem(QString::fromStdString(name));
	//}
}
void igQtTensorWidget::InitTensorAttributes()
{
	//auto manager = iGame::iGameManager::Instance();
	//if (manager->GetCurrentModel() == nullptr) {
	//	return;
	//}
	//auto DataSet = manager->GetCurrentModel()->DataSet;
	//if (DataSet->GetPointData() == nullptr) {
	//	return;
	//}
	//this->Points = DataSet->GetPoints();
	//int tensorIndex = ui->TensorInfoComboBox->currentIndex();
	////findTensor
	//this->TensorAttributes = iGame::iGameFloatArray::SafeDownCast(DataSet->GetPointData()->GetTensors(tensorIndex));
	//this->UpdateComponentsShow(false);
}
void igQtTensorWidget::ShowTensorField()
{
	UpdateEllipsoidGlyphData();
	UpdateEllipsoidGlyphColor();
	Q_EMIT DrawEllipsoidGlyph();
	UpdateScalarsNameList();
	UpdateComponentsShow(true);
}
void igQtTensorWidget::UpdateGlyphScale(double s)
{
	//tensorManager->SetScale(s);
	//UpdateEllipsoidGlyphData();
	//UpdateEllipsoidGlyphColor();
	//Q_EMIT UpdateEllipsoidGlyph();
}
void igQtTensorWidget::UpdateEllipsoidGlyphData()
{
	//int pointNum = this->Points ? this->Points->GetNumberOfPoints() : 0;
	//iGame::Point p;
	//float* t;
	////tensorManager->ReserveEllipsoidData(3 * pointNum);
	//auto manager = iGame::iGameManager::Instance();
	//if (manager->GetCurrentModel() == nullptr) {
	//	return;
	//}
	//auto EllipsoidDrawPointIndexOrders = tensorManager->GetEllipsoidDrawPointIndexOrders();
	//int EllipsoidPointNum = tensorManager->GetNumberOfDrawPoints();
	//EllipsoidGlyphPoints->Reserve(pointNum * 3 * EllipsoidPointNum);
	//EllipsoidGlyphPointOrders->Reserve(pointNum * EllipsoidDrawPointIndexOrders->GetNumberOfValues());
	//clock_t time1 = clock();
	//for (int i = 0; i < pointNum; i++) {
	//	p = this->Points->GetPoint(i);
	//	tensorManager->SetPosition(p);
	//	t = this->TensorAttributes->GetRawPointer() + 9 * i;
	//	tensorManager->SetTensor(t);
	//	auto EllipsoidDrawPoints = tensorManager->GetEllipsoidDrawPoints();
	//	for (int j = 0; j < EllipsoidPointNum * 3; j++) {
	//		EllipsoidGlyphPoints->AddValue(EllipsoidDrawPoints->GetValue(j));
	//	}
	//	int st = i * EllipsoidPointNum;
	//	for (int j = 0; j < EllipsoidDrawPointIndexOrders->GetNumberOfValues(); j++) {
	//		EllipsoidGlyphPointOrders->AddValue(st + EllipsoidDrawPointIndexOrders->GetValue(j));
	//	}
	//}
	//clock_t time2 = clock();
	////std::cout << time2 - time1 << "ms\n";
	//manager->GetCurrentModel()->EllipsoidGlyphPoints = EllipsoidGlyphPoints;
	//manager->GetCurrentModel()->EllipsoidGlyphPointOrders = EllipsoidGlyphPointOrders;



	//std::cout << EllipsoidGlyphPoints->GetNumberOfValues() << '\n';
	//std::cout << EllipsoidGlyphPointOrders->GetNumberOfValues() << '\n';
	//Q_EMIT DrawEllipsoidGlyph(EllipsoidDrawPoints, EllipsoidDrawPointIndexOrders);
}
void igQtTensorWidget::UpdateEllipsoidGlyphColor()
{
	//auto manager = iGame::iGameManager::Instance();
	//if (manager->GetCurrentModel() == nullptr) {
	//	return;
	//}
	//int pointNum = this->Points ? this->Points->GetNumberOfPoints() : 0;
	//int EllipsoidPointNum = tensorManager->GetNumberOfDrawPoints();
	//float rgb[16] = { .0 };
	//if (ui->ScalarInfoComboBox->currentIndex() > 0) {
	//	EllipsoidGlyphColors->Reserve(pointNum * 3 * EllipsoidPointNum);
	//	InitPointGlyphColors();
	//	auto PointColors = this->tensorColorManager->GetColorData();
	//	for (int i = 0; i < pointNum; i++) {
	//		PointColors->GetTuple(i, rgb);
	//		for (int j = 0; j < EllipsoidPointNum; j++) {
	//			EllipsoidGlyphColors->AddValue(rgb[0]);
	//			EllipsoidGlyphColors->AddValue(rgb[1]);
	//			EllipsoidGlyphColors->AddValue(rgb[2]);
	//		}
	//	}
	//}
	//else {
	//	EllipsoidGlyphColors->Clear();
	//}
	//manager->GetCurrentModel()->EllipsoidGlyphColors = this->EllipsoidGlyphColors;
}
void igQtTensorWidget::InitPointGlyphColors()
{
	//auto manager = iGame::iGameManager::Instance();
	//if (manager->GetCurrentModel() == nullptr) {
	//	return;
	//}
	//auto DataSet = manager->GetCurrentModel()->DataSet;
	//if (DataSet->GetPointData() == nullptr) {
	//	return;
	//}
	//int scalarIndex = ui->ScalarInfoComboBox->currentIndex();
	//if (scalarIndex > 0) {
	//	auto scalar = DataSet->GetPointData()->GetScalars(scalarIndex - 1);
	//	//auto scalar = DataSet->GetPointData()->GetScalars("Curvature");
	//	this->tensorColorManager->SetScalarData(scalar, -1);
	//	this->tensorColorManager->InitScalarRange();
	//	this->tensorColorManager->UpdateColorData();
	//}
}
void igQtTensorWidget::GenerateVectorField()
{
//	auto manager = iGame::iGameManager::Instance();
//	if (manager->GetCurrentModel() == nullptr) {
//		return;
//	}
//	auto DataSet = manager->GetCurrentModel()->DataSet;
//	int pointNum = this->Points ? this->Points->GetNumberOfPoints() : 0;
//	float* t;
//	double vector[3];
//	iGame::iGameFloatArray* vectorData = iGame::iGameFloatArray::New();
//	vectorData->SetNumberOfComponents(3);
//	vectorData->SetName(this->TensorAttributes->GetName()+"_PrimaryFeature");
//	for (int i = 0; i < pointNum; i++) {
//		t = this->TensorAttributes->GetRawPointer() + 9 * i;
//		tensorManager->InitTensorEigenData(t);
//		tensorManager->GetEigenVector(0, vector);
//		vectorData->AddTuple3(vector[0], vector[1], vector[2]);
//	}
//	DataSet->GetPointData()->AddVectors(vectorData);
}
