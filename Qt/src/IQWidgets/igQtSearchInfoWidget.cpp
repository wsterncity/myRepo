#include <IQWidgets/igQtSearchInfoWidget.h>

//#include <iGameManager.h>
//#include <iGamePickItem.h>
/**
 * @class   igQtSearchInfoWidget
 * @brief   igQtSearchInfoWidget's brief
 */

igQtSearchInfoWidget::igQtSearchInfoWidget(QWidget* parent) :QWidget(parent), ui(new Ui::SearchInfo) {
	//ui->setupUi(this);
	//connect(ui->btn_SearchData, SIGNAL(clicked(bool)), this, SLOT(searchData()));
	//connect(ui->btn_Reset, SIGNAL(clicked(bool)), this, SLOT(reset()));
	//connect(ui->btn_Clear, SIGNAL(clicked(bool)), this, SLOT(clear()));
	//connect(ui->comboBox_DataProducer, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateElementType()));
	//connect(ui->comboBox_ElementType, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateElementItem()));
	//connect(ui->comboBox_ElementItem, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateSearchWay()));
	//connect(ui->comboBox_SearchWay, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateLineEdit()));
	//ui->comboBox_DataProducer->setCurrentIndex(0);
	//searchedDataIdInfo = iGame::iGameIntArray::New();

	//connect(ui->btn_FirstPage, &QPushButton::clicked, this, [&]() {
	//	this->pageIndex = 0;
	//	updateDataToTable();
	//	});
	//connect(ui->btn_LastPage, &QPushButton::clicked, this, [&]() {
	//	pageIndex = qBound(0, pageIndex - 1, pageNum - 1);
	//	updateDataToTable();
	//	});
	//connect(ui->btn_NextPage, &QPushButton::clicked, this, [&]() {
	//	pageIndex = qBound(0, pageIndex + 1, pageNum - 1);
	//	updateDataToTable();
	//	});
	//connect(ui->btn_FinalPage, &QPushButton::clicked, this, [&]() {
	//	this->pageIndex = pageNum - 1;
	//	updateDataToTable();
	//	});
	//QRegExp rx("(\\d?){0,10}");
	//ui->lineEdit_CustomPage->setValidator(new QRegExpValidator(rx, this));
	//connect(ui->lineEdit_CustomPage, &QLineEdit::editingFinished, this, [&]() {
	//	std::stringstream ss(ui->lineEdit_CustomPage->text().toStdString());
	//	ss >> pageIndex;
	//	pageIndex = qBound(0, pageIndex - 1, pageNum - 1);
	//	updateDataToTable();
	//	});

}


void igQtSearchInfoWidget::updateDataProducer()
{
	//ui->comboBox_DataProducer->clear();
	//ui->comboBox_DataProducer->addItem("(none)");
	//auto manager = iGame::iGameManager::Instance();
	//auto modelList = manager->GetModelList();
	//for (int i = 0; i < modelList.size(); i++) {
	//	auto filename = modelList[i]->filePath.substr(modelList[i]->filePath.find_last_of('/') + 1);
	//	ui->comboBox_DataProducer->addItem(QString::fromStdString(filename));
	//}
	//return;
}
void igQtSearchInfoWidget::updateElementType()
{
	this->dataProducerIndex = ui->comboBox_DataProducer->currentIndex();
	ui->comboBox_ElementType->clear();
	if (this->dataProducerIndex) {
		ui->comboBox_ElementType->addItem("Point");
		ui->comboBox_ElementType->addItem("Cell");
	}
}
void igQtSearchInfoWidget::updateElementItem()
{
	this->elementType = getElementType(ui->comboBox_ElementType->currentText());
	ui->comboBox_ElementItem->clear();
	if (this->elementType == POINT) {
		ui->comboBox_ElementItem->addItem("Id");
		ui->comboBox_ElementItem->addItem("Position");
		//scalar vector every data
	}
	else if (this->elementType == CELL) {
		ui->comboBox_ElementItem->addItem("Id");
		//scalar vector every data
	}
}
void igQtSearchInfoWidget::updateSearchWay()
{
	this->elementItem = getElementItem(ui->comboBox_ElementItem->currentText());
	ui->comboBox_SearchWay->clear();
	switch (elementItem)
	{
	case igQtSearchInfoWidget::ID:
		ui->comboBox_SearchWay->addItem("is");
		ui->comboBox_SearchWay->addItem("is in range");
		ui->comboBox_SearchWay->addItem("is <=");
		ui->comboBox_SearchWay->addItem("is >=");
		ui->comboBox_SearchWay->addItem("is max");
		ui->comboBox_SearchWay->addItem("is min");
		break;
	case igQtSearchInfoWidget::SCALAR:
		ui->comboBox_SearchWay->addItem("is");
		ui->comboBox_SearchWay->addItem("is in range");
		ui->comboBox_SearchWay->addItem("is <=");
		ui->comboBox_SearchWay->addItem("is >=");
		ui->comboBox_SearchWay->addItem("is max");
		ui->comboBox_SearchWay->addItem("is min");
		ui->comboBox_SearchWay->addItem("is nearst to");
		break;
	case igQtSearchInfoWidget::POSITION:
		ui->comboBox_SearchWay->addItem("is nearst to");
		break;
	default:
		break;
	}
}
void igQtSearchInfoWidget::updateLineEdit()
{
	this->searchWay = getSearchWay(ui->comboBox_SearchWay->currentText());
	QRegExp rx;
	rx = getElementItem(ui->comboBox_ElementItem->currentText()) == ID ? QRegExp("(\\d?){0,10}") : QRegExp("^-?\\d+(\\.\\d+)?([eE][-+]?\\d+)?$");
	ui->lineEdit_Value0->setValidator(new QRegExpValidator(rx, this));
	ui->lineEdit_Value1->setValidator(new QRegExpValidator(rx, this));
	ui->lineEdit_Value2->setValidator(new QRegExpValidator(rx, this));

	switch (searchWay)
	{
	case igQtSearchInfoWidget::IS:
		ui->label_To->hide();
		ui->lineEdit_Value0->setPlaceholderText("ID");
		ui->lineEdit_Value0->show();
		ui->lineEdit_Value1->hide();
		ui->lineEdit_Value2->hide();
		ui->lineEdit_Epsilon->hide();
		break;
	case igQtSearchInfoWidget::ISINRANGE:
		ui->label_To->show();
		ui->lineEdit_Value0->setPlaceholderText("MinID");
		ui->lineEdit_Value1->setPlaceholderText("MaxID");
		ui->lineEdit_Value0->show();
		ui->lineEdit_Value1->show();
		ui->lineEdit_Value2->hide();
		ui->lineEdit_Epsilon->hide();
		break;
	case igQtSearchInfoWidget::ISBIGGER:
		ui->label_To->hide();
		ui->lineEdit_Value0->setPlaceholderText("MinID");
		ui->lineEdit_Value0->show();
		ui->lineEdit_Value1->hide();
		ui->lineEdit_Value2->hide();
		ui->lineEdit_Epsilon->hide();
		break;
	case igQtSearchInfoWidget::ISSMALLER:
		ui->label_To->hide();
		ui->lineEdit_Value0->setPlaceholderText("MaxID");
		ui->lineEdit_Value0->show();
		ui->lineEdit_Value1->hide();
		ui->lineEdit_Value2->hide();
		ui->lineEdit_Epsilon->hide();
		break;
	case igQtSearchInfoWidget::ISMIN:
		ui->label_To->hide();
		ui->lineEdit_Value0->hide();
		ui->lineEdit_Value1->hide();
		ui->lineEdit_Value2->hide();
		ui->lineEdit_Epsilon->hide();
		break;
	case igQtSearchInfoWidget::ISMAX:
		ui->label_To->hide();
		ui->lineEdit_Value0->hide();
		ui->lineEdit_Value1->hide();
		ui->lineEdit_Value2->hide();
		ui->lineEdit_Epsilon->hide();
		break;
	case igQtSearchInfoWidget::ISNEARSTTO:
		ui->label_To->hide();
		ui->lineEdit_Value0->setPlaceholderText("x");
		ui->lineEdit_Value1->setPlaceholderText("y");
		ui->lineEdit_Value2->setPlaceholderText("z");
		ui->lineEdit_Value0->show();
		ui->lineEdit_Value1->show();
		ui->lineEdit_Value2->show();
		ui->lineEdit_Epsilon->show();
		break;
	default:
		break;
	}
}
igQtSearchInfoWidget::ElementType igQtSearchInfoWidget::getElementType(QString str)
{
	if (str == "Point")return POINT;
	if (str == "Cell")return CELL;
	return ELEMENTTYPE;
}
igQtSearchInfoWidget::ElementItem igQtSearchInfoWidget::getElementItem(QString str)
{
	if (str == "Id")return ID;
	if (str == "Position")return POSITION;
	//return scalar vector such data
	return ELEMENTITEM;
}
igQtSearchInfoWidget::SearchWay igQtSearchInfoWidget::getSearchWay(QString str)
{
	if (str == "is")return IS;
	if (str == "is in range")return ISINRANGE;
	if (str == "is >=")return ISBIGGER;
	if (str == "is <=")return ISSMALLER;
	if (str == "is max")return ISMAX;
	if (str == "is min")return ISMIN;
	if (str == "is nearst to")return ISNEARSTTO;
	return SEARCHWAY;
}

void igQtSearchInfoWidget::updateValuesData()
{
	std::stringstream ss(ui->lineEdit_Value0->text().toStdString());
	ss >> this->value[0];
	ss.clear();
	ss.str(ui->lineEdit_Value1->text().toStdString());
	ss >> this->value[1];
	ss.clear();
	ss.str(ui->lineEdit_Value2->text().toStdString());
	ss >> this->value[2];
}

void igQtSearchInfoWidget::searchData()
{

	//updateValuesData();
	//searchedDataIdInfo->Clear();
	//switch (this->elementItem)
	//{
	//case igQtSearchInfoWidget::ID:
	//	searchDataWithId();
	//	break;
	//case igQtSearchInfoWidget::SCALAR:
	//	break;
	//case igQtSearchInfoWidget::POSITION:
	//	searchDataWithPosition();

	//	break;
	//default:
	//	break;
	//}

	//resetPageInfo();
	//updateDataToTable();
	//drawSearchedPoint();
	//this->tmpDataProducerIndex = this->dataProducerIndex;
	//this->tmpElementType = this->elementType;
	//this->tmpElementItem = this->elementItem;
	//this->tmpSearchWay = this->searchWay;

}
void igQtSearchInfoWidget::updateDataToTable()
{
	updateShowedDataStartAndEnd();
	std::string str = std::to_string(pageIndex + 1) + " / " + std::to_string(pageNum);
	ui->label_CurrentPage->setText(QString::fromStdString(str));
	ui->tableWidget_SearchInfo->clear();
	ui->tableWidget_SearchInfo->setRowCount(dataEd - dataSt);

	switch (elementType)
	{
	case igQtSearchInfoWidget::POINT:
		updateDataToTableWithPointType();
		break;
	case igQtSearchInfoWidget::TRIANGEL:
		break;
	case igQtSearchInfoWidget::QUAD:
		break;
	case igQtSearchInfoWidget::POLYON:
		break;
	case igQtSearchInfoWidget::TETRA:
		break;
	case igQtSearchInfoWidget::HEXAHEDRON:
		break;
	case igQtSearchInfoWidget::PRISM:
		break;
	case igQtSearchInfoWidget::PYRAMID:
		break;
	case igQtSearchInfoWidget::ELEMTTYPESUM:
		break;
	default:
		break;
	}

}
void igQtSearchInfoWidget::reset()
{
	ui->comboBox_DataProducer->setCurrentIndex(this->tmpDataProducerIndex);
	ui->comboBox_ElementType->setCurrentIndex(this->tmpElementType);
	ui->comboBox_ElementItem->setCurrentIndex(this->tmpElementItem);
	ui->comboBox_SearchWay->setCurrentIndex(this->tmpSearchWay);
}
void igQtSearchInfoWidget::searchDataWithPosition()
{
	//auto manager = iGame::iGameManager::Instance();
	//iGame::iGameManager::Model* dataModel = manager->GetModelList()[dataProducerIndex - 1];
	//if (dataModel == nullptr)return;
	//switch (this->searchWay) {
	//case igQtSearchInfoWidget::ISNEARSTTO:
	//{
	//	auto app = iGame::iGamePickItem::New();
	//	app->setDataSet(dataModel->DataSet);
	//	std::stringstream ss(ui->lineEdit_Epsilon->text().toStdString());
	//	ss >> this->epsilon;
	//	app->setEpsilon(this->epsilon);
	//	int ret = app->PickPointWithNearstPosition(value[0], value[1], value[2]);
	//	if (ret != -1) {
	//		searchedDataIdInfo->AddValue(ret);
	//	}
	//}
	//break;
	//default:
	//	break;
	//}


}

void igQtSearchInfoWidget::searchDataWithId()
{
	//auto manager = iGame::iGameManager::Instance();
	//iGame::iGameManager::Model* dataModel = manager->GetModelList()[dataProducerIndex - 1];
	//if (dataModel == nullptr)return;
	//int minID = 0;
	//int maxID = 0;
	//switch (elementType)
	//{
	//case igQtSearchInfoWidget::POINT:
	//	maxID = dataModel->DataSet->GetNumberOfPoints() - 1;
	//	break;
	//case igQtSearchInfoWidget::TRIANGEL:
	//	maxID = dataModel->DataSet->GetNumberOfTriangles() - 1;
	//	break;
	//case igQtSearchInfoWidget::QUAD:
	//	maxID = dataModel->DataSet->GetNumberOfQuads() - 1;
	//	break;
	//case igQtSearchInfoWidget::POLYON:
	//	break;
	//case igQtSearchInfoWidget::TETRA:
	//	break;
	//case igQtSearchInfoWidget::HEXAHEDRON:
	//	break;
	//case igQtSearchInfoWidget::PRISM:
	//	break;
	//case igQtSearchInfoWidget::PYRAMID:
	//	break;
	//case igQtSearchInfoWidget::ELEMTTYPESUM:
	//	break;
	//default:
	//	break;
	//}

	//switch (searchWay)
	//{
	//case igQtSearchInfoWidget::IS:
	//	searchedDataIdInfo->AddValue((int)value[0]);
	//	break;
	//case igQtSearchInfoWidget::ISINRANGE:
	//{
	//	int st = qBound(0, (int)value[0], maxID);
	//	int ed = qBound(0, (int)value[1], maxID);
	//	searchedDataIdInfo->Resize(ed - st + 1);
	//	for (int i = st; i <= ed; i++) {
	//		searchedDataIdInfo->AddValue(i);
	//	}
	//}
	//break;
	//case igQtSearchInfoWidget::ISSMALLER:
	//{
	//	int n = std::min(maxID, (int)value[0]) + 1;
	//	searchedDataIdInfo->Resize(n);
	//	for (int i = 0; i < n; i++) {
	//		searchedDataIdInfo->AddValue(i);
	//	}
	//}
	//break;
	//case igQtSearchInfoWidget::ISBIGGER:
	//{
	//	int st = std::max(0, (int)value[0]);
	//	searchedDataIdInfo->Resize(maxID - st + 1);
	//	for (int i = st; i <= maxID; i++) {
	//		searchedDataIdInfo->AddValue(i);
	//	}
	//}
	//break;
	//case igQtSearchInfoWidget::ISMIN:
	//	searchedDataIdInfo->AddValue(0);
	//	break;
	//case igQtSearchInfoWidget::ISMAX:
	//	searchedDataIdInfo->AddValue(maxID);
	//	break;
	//default:
	//	break;
	//}
}
void igQtSearchInfoWidget::clear()
{
	//ui->comboBox_DataProducer->setCurrentIndex(0);
	//ui->tableWidget_SearchInfo->clear();
	//searchedDataIdInfo->Clear();
}
void igQtSearchInfoWidget::resetPageInfo()
{
	//this->pageIndex = 0;
	//this->pageNum = 1 + (this->searchedDataIdInfo->GetNumberOfValues() - 1) / this->dataNumPerPage;
}
void igQtSearchInfoWidget::updateShowedDataStartAndEnd()
{
	//this->dataSt = pageIndex * dataNumPerPage;
	//this->dataEd = std::min(dataSt + dataNumPerPage, searchedDataIdInfo->GetNumberOfValues());
}
void igQtSearchInfoWidget::searchDataWithFramePlane()
{
	resetPageInfo();
	updateDataToTable();
}
void igQtSearchInfoWidget::searchDataWithRay(iGame::Vector3f startPoint, iGame::Vector3f dir)
{
	//auto app = iGame::iGamePickItem::New();
	//app->setEpsilon(0.01f);
	//int ret = app->PickPointWithNearstRay(startPoint, dir);
	//if (ret != -1) {
	//	this->searchedDataIdInfo->AddValue(ret);
	//}
	////std::cout << startPoint[0] << " " << startPoint[1] << " " << startPoint[2] << "\n";
	////std::cout << dir[0] << " " << dir[1] << " " << dir[2] << "\n";
	//this->elementType = POINT;
	//this->dataProducerIndex = iGame::iGameManager::Instance()->currentModelIdx + 1;
	//resetPageInfo();
	//updateDataToTable();
	//drawSearchedPoint();
}
void igQtSearchInfoWidget::drawSearchedPoint()
{
	//int n = this->searchedDataIdInfo->GetNumberOfValues();
	//auto dataModel = iGame::iGameManager::Instance()->GetModelList()[this->dataProducerIndex - 1];
	//iGame::iGameFloatArray* drawPoints = iGame::iGameFloatArray::New();
	//drawPoints->SetNumberOfComponents(3);
	//drawPoints->Reserve(n);
	//auto Points = dataModel->DataSet->GetPoints();
	//for (int i = 0; i < n; i++) {
	//	auto p = Points->GetPoint(this->searchedDataIdInfo->GetValue(i));
	//	drawPoints->AddTuple3(p[0], p[1], p[2]);
	//}
	//Q_EMIT  showSearchedPoint(drawPoints);
}

void igQtSearchInfoWidget::updateDataToTableWithCellType()
{

}

void igQtSearchInfoWidget::updateDataToTableWithPointType()
{
	//ui->tableWidget_SearchInfo->clear();
	//if (searchedDataIdInfo->GetNumberOfValues() == 0)return;
	//auto manager = iGame::iGameManager::Instance();
	//if(dataProducerIndex==0)return;
	//iGame::iGameManager::Model* dataModel = manager->GetModelList()[dataProducerIndex - 1];
	//if (dataModel == nullptr)return;
	//auto points = dataModel->DataSet->GetPoints();
	//auto pointsData = dataModel->DataSet->GetPointData();
	//iGame::Point p;
	//ui->tableWidget_SearchInfo->setColumnCount(4);
	//ui->tableWidget_SearchInfo->setHorizontalHeaderItem(0, new  QTableWidgetItem("ID"));
	//ui->tableWidget_SearchInfo->setHorizontalHeaderItem(1, new  QTableWidgetItem("X"));
	//ui->tableWidget_SearchInfo->setHorizontalHeaderItem(2, new  QTableWidgetItem("Y"));
	//ui->tableWidget_SearchInfo->setHorizontalHeaderItem(3, new  QTableWidgetItem("Z"));
	//int row = 0;
	//int col = 0;
	//int id = 0;
	//for (int i = dataSt; i < dataEd; i++) {
	//	id = searchedDataIdInfo->GetValue(i);
	//	ui->tableWidget_SearchInfo->setItem(row, 0, new  QTableWidgetItem(QString::fromStdString(std::to_string(id))));
	//	p = points->GetPoint(id);
	//	ui->tableWidget_SearchInfo->setItem(row, 1, new  QTableWidgetItem(QString::fromStdString(std::to_string(p[0]))));
	//	ui->tableWidget_SearchInfo->setItem(row, 2, new  QTableWidgetItem(QString::fromStdString(std::to_string(p[1]))));
	//	ui->tableWidget_SearchInfo->setItem(row, 3, new  QTableWidgetItem(QString::fromStdString(std::to_string(p[2]))));
	//	row++;
	//}
	//col += 4;
	//float values[16] = { 0 };
	//if (pointsData != nullptr) {
	//	auto pointScalars = pointsData->GetAllScalars();
	//	for (int it = 0; it < pointScalars.size(); it++) {
	//		auto scalarData = pointScalars[it].Array;
	//		int size = scalarData->GetNumberOfComponents();
	//		std::string name = scalarData->GetName();
	//		if (size == 1) {
	//			ui->tableWidget_SearchInfo->insertColumn(col);
	//			ui->tableWidget_SearchInfo->setHorizontalHeaderItem(col, new  QTableWidgetItem(QString::fromStdString(name)));
	//			row = 0;
	//			for (int i = dataSt; i < dataEd; i++) {
	//				id = searchedDataIdInfo->GetValue(i);
	//				ui->tableWidget_SearchInfo->setItem(row, col, new  QTableWidgetItem(QString::fromStdString(std::to_string(scalarData->GetValueF(id)))));
	//				row++;
	//			}
	//		}
	//		else {
	//			std::string demension[4] = { "_X","_Y","_Z","_U" };
	//			for (int i = 0; i < size; i++) {
	//				ui->tableWidget_SearchInfo->insertColumn(col + i);
	//				ui->tableWidget_SearchInfo->setHorizontalHeaderItem(col + i, new  QTableWidgetItem(QString::fromStdString(name + demension[i])));
	//			}
	//			row = 0;
	//			for (int i = dataSt; i < dataEd; i++) {
	//				id = searchedDataIdInfo->GetValue(i);
	//				scalarData->GetTupleF(id, values);
	//				for (int j = 0; j < size; j++) {
	//					ui->tableWidget_SearchInfo->setItem(row, col + j, new  QTableWidgetItem(QString::fromStdString(std::to_string(values[j]))));
	//				}
	//				row++;
	//			}
	//		}
	//		col += size;
	//	}

	//	auto pointVectors = pointsData->GetAllVectors();
	//	for (int it = 0; it < pointVectors.size(); it++) {
	//		auto vectorData = pointVectors[it].Array;
	//		int size = vectorData->GetNumberOfComponents();
	//		std::string name = vectorData->GetName();
	//		std::string demension[4] = { "_X","_Y","_Z","_U" };
	//		for (int i = 0; i < size; i++) {
	//			ui->tableWidget_SearchInfo->insertColumn(col + i);
	//			ui->tableWidget_SearchInfo->setHorizontalHeaderItem(col + i, new  QTableWidgetItem(QString::fromStdString(name + demension[i])));
	//		}
	//		row = 0;
	//		for (int i = dataSt; i < dataEd; i++) {
	//			id = searchedDataIdInfo->GetValue(i);
	//			vectorData->GetTupleF(id, values);
	//			for (int j = 0; j < size; j++) {
	//				ui->tableWidget_SearchInfo->setItem(row, col + j, new  QTableWidgetItem(QString::fromStdString(std::to_string(values[j]))));
	//			}
	//			row++;
	//		}
	//		col += size;
	//	}
	//}
}