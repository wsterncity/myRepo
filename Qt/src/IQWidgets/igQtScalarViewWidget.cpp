#include <IQWidgets/igQtScalarViewWidget.h>

//#include <iGameManager.h>
//#include <iGameModelColorManager.h>
/**
 * @class   igQtScalarViewWidget
 * @brief   igQtScalarViewWidget's brief
 */
igQtScalarViewWidget::igQtScalarViewWidget(QWidget* parent) : QWidget(parent), ui(new Ui::ScalarView) 
{
	//ui->setupUi(this);
	//modelColorManager = iGame::iGameModelColorManager::Instance();
	//SetCustomScaleRangeWidget = new	QWidget;
	//SetCustomScaleRangeUi = new Ui::SetCustomScaleRange;
	//SetCustomScaleRangeUi->setupUi(SetCustomScaleRangeWidget);
	//SetCustomScaleRangeWidget->hide();
	//ui->widget_DataRangeSlider->hide();
	//connect(ui->scalarInfoComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(showScalarItem()));
	//connect(ui->scalarItemComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(showScalarView()));
	//connect(ui->btn_EditColorMap, &QPushButton::clicked, this, &igQtScalarViewWidget::editColorBar);
	//connect(ui->btn_RescaleRange, &QPushButton::clicked, this, &igQtScalarViewWidget::rescaleRange);
	//connect(ui->btn_SetRange, &QPushButton::clicked, this, &igQtScalarViewWidget::showCustomScaleRangeWidget);
	//connect(ui->btn_IsShowColorLegend, &QPushButton::clicked, this, &igQtScalarViewWidget::isShowColorLegend);
	//connect(ui->widget_DataRangeSlider, &igQtDataRangeSlider::DataRangeChanged, this, [&](float _min, float _max) {
	//	auto ColorMap = modelColorManager->GetColorMapper();
	//	ColorMap->SetRange(_min, _max);
	//	drawModelWithScalarData();
	//	});
	//connect(SetCustomScaleRangeUi->btnRescale, &QPushButton::clicked, this, &igQtScalarViewWidget::setCustomScaleRange);
	//connect(SetCustomScaleRangeUi->btnCancle, &QPushButton::clicked, this, [&]() {this->SetCustomScaleRangeWidget->hide(); });

}
void igQtScalarViewWidget::getScalarsName()
{
	//ui->scalarInfoComboBox->clear();
	//ui->scalarInfoComboBox->addItem("Solider");
	//auto manager = iGame::iGameManager::Instance();
	//if (manager->GetModelList().size() == 0)return;
	//auto model = manager->GetCurrentModel();
	//auto dataset = model->DataSet;
	//scalarInfo.clear();
	//if (dataset->GetPointData() == nullptr)return;
	//auto attributes = dataset->GetPointData()->GetAllScalars();
	//for (int i = 0; i < attributes.size(); i++) {
	//	auto data = dataset->GetPointData()->GetScalars(i);
	//	std::string name = data->GetName();
	//	int size = data->GetNumberOfComponents();
	//	scalarInfo[name] = size;
	//}
	//for (auto it : scalarInfo) {
	//	ui->scalarInfoComboBox->addItem(QString::fromStdString(it.first));
	//}
}
void igQtScalarViewWidget::showScalarItem()
{
 //   this->currentSelectedScalarIdx = ui->scalarInfoComboBox->currentIndex();
 //   modelColorManager->SetCurrentSelectedScalarIdx(currentSelectedScalarIdx - 1);
	//auto scalarName = ui->scalarInfoComboBox->currentText().toStdString();
	//int size = 0;
	//for (auto& [_name, _size] : scalarInfo) {
	//	if (_name == scalarName) {
	//		size = _size;
	//		break;
	//	}
	//}
	////std::cout << scalarName << std::endl;
	////std::cout << size << std::endl;
	//ui->scalarItemComboBox->clear();
	//if (size > 1) ui->scalarItemComboBox->addItem(QString::fromStdString("magnitude"));
	//if (size > 0) ui->scalarItemComboBox->addItem(QString::fromStdString("x"));
	//if (size > 1) ui->scalarItemComboBox->addItem(QString::fromStdString("y"));
	//if (size > 2) ui->scalarItemComboBox->addItem(QString::fromStdString("z"));
	//if (size > 3) ui->scalarItemComboBox->addItem(QString::fromStdString("u"));
}
void igQtScalarViewWidget::loadScalarData()
{
 //   this->currentSelectedScalarIdx = ui->scalarInfoComboBox->currentIndex();
	//this->scalarName = ui->scalarInfoComboBox->currentText().toStdString();
 //   this->drawItem = ui->scalarItemComboBox->currentIndex();
 //   modelColorManager->SetCurrentSelectedScalarIdx(currentSelectedScalarIdx - 1);
 //   modelColorManager->SetCurrentScalarComponentIdx(drawItem - 1);
 //   this->scalarData = nullptr;
	//if (drawItem == -1 || scalarName == "Solider")return;
	//auto manager = iGame::iGameManager::Instance();
	//if (manager->GetModelList().size() == 0)return;
	//auto PointData = manager->GetCurrentModel()->DataSet->GetPointData();
	//if (PointData != nullptr){
	//	this->scalarData = PointData->GetScalars(scalarName);
	//}
	//return;
}
void igQtScalarViewWidget::initScalarRange() {
	//if (this->scalarData == nullptr) {
	//	ui->widget_DataRangeSlider->hide();
	//	return;
	//}
	//this->modelColorManager->InitScalarRange();
	//auto ColorMap = modelColorManager->GetColorMapper();
	//this->scalarMin = ColorMap->GetRange()[0];
	//this->scalarMax = ColorMap->GetRange()[1];
	//ui->widget_DataRangeSlider->updateMinAndMax(scalarMin, scalarMax);
	//ui->widget_DataRangeSlider->show();
}
void igQtScalarViewWidget::showScalarView()
{
	//loadScalarData();
	//this->modelColorManager->SetScalarData(this->scalarData, drawItem - 1);
	//initScalarRange();
	//drawModelWithScalarData();

	//std::string str = scalarName + "\nMin Value : " + std::to_string(scalarMin) + "\nMax Value : " + std::to_string(scalarMax);
	//ui->labelScalarInfo->clear();
	//if (this->scalarData==nullptr)return;
	//ui->labelScalarInfo->setText(QString::fromStdString(str));
}
void igQtScalarViewWidget::drawModelWithScalarData()
{
	//modelColorManager->UpdateDrawInfoWithColor();
	//Q_EMIT updateCurrentModelColor();
}
void igQtScalarViewWidget::editColorBar()
{
	Q_EMIT ChangeShowColorManager();
}
void igQtScalarViewWidget::rescaleRange()
{
	//auto ColorMap = modelColorManager->GetColorMapper();
	//ColorMap->SetRange(scalarMin, scalarMax);
	//ui->widget_DataRangeSlider->updateMinAndMax(scalarMin, scalarMax);
	//drawModelWithScalarData();
}

void igQtScalarViewWidget::setCustomScaleRange()
{
	//auto ColorMap = modelColorManager->GetColorMapper();
	//QString strMin = SetCustomScaleRangeUi->lineEdit_min->text();
	//QString strMax = SetCustomScaleRangeUi->lineEdit_max->text();
	//float min = 0.0, max = 0.0;
	//std::stringstream ssmin(strMin.toStdString()), ssmax(strMax.toStdString());
	//ssmin >> min;
	//ssmax >> max;
	//ColorMap->SetRange(min, max);
	//ui->widget_DataRangeSlider->updateMinAndMax(min, max);
	//drawModelWithScalarData();
	//this->SetCustomScaleRangeWidget->hide();
}

void igQtScalarViewWidget::showCustomScaleRangeWidget()
{
	//QRegExp rx("^-?\\d+(\\.\\d+)?([eE][-+]?\\d+)?$");
	//SetCustomScaleRangeUi->lineEdit_min->setValidator(new QRegExpValidator(rx, this));
	//SetCustomScaleRangeUi->lineEdit_max->setValidator(new QRegExpValidator(rx, this));
	//auto min = modelColorManager->GetColorMapper()->GetRange()[0];
	//auto max = modelColorManager->GetColorMapper()->GetRange()[1];
	//SetCustomScaleRangeUi->lineEdit_min->setText(QString::fromStdString(std::to_string(min)));
	//SetCustomScaleRangeUi->lineEdit_max->setText(QString::fromStdString(std::to_string(max)));
	//this->SetCustomScaleRangeWidget->show();
}
void igQtScalarViewWidget::isShowColorLegend()
{
	Q_EMIT changeColorBarShow();
}

int igQtScalarViewWidget::getCurrentSelectedScalarIdx() {
    return currentSelectedScalarIdx;
}
