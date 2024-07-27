//
// Created by m_ky on 2024/4/10.
//

#include <IQCore/igQtMainWindow.h>
#include <IQCore/igQtFileLoader.h>
#include <IQWidgets/igQtModelDrawWidget.h>

#include <IQComponents/igQtModelListView.h>
#include <IQWidgets/ColorManager/igQtBasicColorAreaWidget.h>
#include <IQWidgets/ColorManager/igQtColorManagerWidget.h>
#include <IQComponents/igQtFilterDialogDockWidget.h>
#include <IQComponents/igQtProgressBarWidget.h>
#include <IQWidgets/igQtTensorWidget.h>



igQtMainWindow::igQtMainWindow(QWidget* parent) : 
	QMainWindow(parent), 
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	rendererWidget = new igQtModelDrawWidget(this);
	fileLoader = new igQtFileLoader(this);
	this->setCentralWidget(rendererWidget);
	this->ColorManagerWidget = new igQtColorManagerWidget;
	ColorManagerWidget->setGeometry(400, 500, 780, 1000);
	ui->dockWidget_ScalarField->hide();
	ui->dockWidget_VectorField->hide();
	ui->dockWidget_FlowField->hide();
	ui->dockWidget_TensorField->hide();
	ui->dockWidget_SearchInfo->hide();
	ui->dockWidget_QualityDetection->hide();
	ui->dockWidget_EditMode->hide();
	ui->dockWidget_Animation->hide();
	ui->dockWidget_ModelList->setWidget(ui->modelTreeView);
	// Setup default GUI layout.
	this->setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);
	this->setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);
	this->setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::North);
	// Set up the dock window corners to give the vertical docks more room.
	this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	initToolbarComponent();
	initAllComponents();
	initAllFilters();
	updateRecentFilePaths();

}
void igQtMainWindow::initToolbarComponent()
{
	viewStyleCombox = new QComboBox(this);
	viewStyleCombox->addItem("Points");
	viewStyleCombox->addItem("WireFrame");
	viewStyleCombox->addItem("Surface");
	viewStyleCombox->addItem("Surface With Edegs");
	viewStyleCombox->addItem("Volume");
	viewStyleCombox->addItem("Volume With Edegs");

	viewStyleCombox->setStyleSheet("QComboBox {"
		"background-color: #f0f0f0;"
		"color: #202020;"              // 设置文本颜色为浅白色
		"border: 1px solid #ffffff;"   // 设置边框样式为灰色实线边框
		"padding: 5px;"                // 设置内边距
		"font-size: 16px;"              // 设置下拉菜单项字体大小为14px
		"}"
		"QComboBox QAbstractItemView {"
		"font-family: Arial;"           // 设置下拉菜单项字体为Arial
		"color: #404040;"               // 设置下拉菜单项字体颜色为浅灰色
		"}"
		"QComboBox::drop-down {"
		"subcontrol-origin: padding;"
		"subcontrol-position: top right;"
		"width: 20px;"
		"border-left: 1px solid #202020;"
		"border-color: #eeeeee;"
		"}"
	);

	connect(viewStyleCombox, SIGNAL(currentIndexChanged(QString)), this, SLOT(ChangeViewStyle()));
	ui->toolBar_meshview->addWidget(viewStyleCombox);
	
	attributeViewIndexCombox = new QComboBox(this);
	attributeViewIndexCombox->addItem("None        ");
	attributeViewIndexCombox->setStyleSheet("QComboBox {"
		"background-color: #f0f0f0;"
		"color: #202020;"              // 设置文本颜色为浅白色
		"border: 1px solid #ffffff;"   // 设置边框样式为灰色实线边框
		"padding: 5px;"                // 设置内边距
		"font-size: 16px;"              // 设置下拉菜单项字体大小为14px
		"}"
		"QComboBox QAbstractItemView {"
		"font-family: Arial;"           // 设置下拉菜单项字体为Arial
		"color: #404040;"               // 设置下拉菜单项字体颜色为浅灰色
		"}"
		"QComboBox::drop-down {"
		"subcontrol-origin: padding;"
		"subcontrol-position: top right;"
		"width: 20px;"
		"border-left: 1px solid #202020;"
		"border-color: #eeeeee;"
		"}"
	);

	connect(attributeViewIndexCombox, SIGNAL(activated(int)), this, SLOT(ChangeScalarView()));
	ui->toolBar_attribute_view_index->addWidget(attributeViewIndexCombox);


	attributeViewDimCombox = new QComboBox(this);
	attributeViewDimCombox->addItem("magnitude");
	attributeViewDimCombox->setStyleSheet("QComboBox {"
		"background-color: #f0f0f0;"
		"color: #202020;"              // 设置文本颜色为浅白色
		"border: 1px solid #ffffff;"   // 设置边框样式为灰色实线边框
		"padding: 5px;"                // 设置内边距
		"font-size: 16px;"              // 设置下拉菜单项字体大小为14px
		"}"
		"QComboBox QAbstractItemView {"
		"font-family: Arial;"           // 设置下拉菜单项字体为Arial
		"color: #404040;"               // 设置下拉菜单项字体颜色为浅灰色
		"}"
		"QComboBox::drop-down {"
		"subcontrol-origin: padding;"
		"subcontrol-position: top right;"
		"width: 10px;"
		"border-left: 1px solid #202020;"
		"border-color: #eeeeee;"
		"}"
	);

	connect(attributeViewDimCombox, SIGNAL(activated(int)), this, SLOT(ChangeScalarViewDim()));
	ui->toolBar_attribute_view_dim->addWidget(attributeViewDimCombox);
}
void igQtMainWindow::initAllComponents()
{

	//// init ProgressBar
	//progressBarWidget = new igQtProgressBarWidget(this);
	//this->statusBar()->addPermanentWidget(progressBarWidget);

	//connect(ui->action_SaveScreenShot, &QAction::triggered, rendererWidget, &igQtModelDrawWidget::SaveScreenShoot);
	connect(ui->action_LoadFile, &QAction::triggered, fileLoader, &igQtFileLoader::LoadFile);
	//connect(ui->action_SaveMesh, &QAction::triggered, fileLoader, &igQtFileLoader::SaveFile);
	//connect(ui->action_SaveMeshAs, &QAction::triggered, fileLoader, &igQtFileLoader::SaveFileAs);
	//connect(ui->action_CopyMesh, &QAction::triggered, this, [&]() {
	//	iGame::iGameManager::Instance()->CopyMesh();
	//	});
	//connect(ui->action_RecoverMesh, &QAction::triggered, this, [&]() {
	//	iGame::iGameManager::Instance()->RecoverMesh();
	//	});
	//connect(ui->action_ResetCenter, &QAction::triggered, this, [&]() {
	//	iGame::iGameManager::Instance()->ResetCenter();
	//	rendererWidget->update();
	//	});
	//connect(ui->action_PickCenter, &QAction::triggered, this, [&]() {
	//	float x = -1.0, y = -1.0, z = -1.0;
	//	iGame::iGameManager::Instance()->UpdateCenter(x, y, z);
	//	rendererWidget->update();
	//	});
	connect(ui->action_DeleteMesh, &QAction::triggered, ui->modelTreeView, &igQtModelListView::DeleteCurrentFile);
	connect(ui->action_DeleteMesh, &QAction::triggered, this, &igQtMainWindow::updateCurrentSceneWidget);
	//connect(ui->action_NextMesh, &QAction::triggered, ui->modelTreeView, &igQtModelListView::ChangeSelected2NextItem);
	//connect(ui->action_NextMesh, &QAction::triggered, rendererWidget, &igQtModelDrawWidget::changeCurrentModel2Next);
	//connect(ui->action_LastMesh, &QAction::triggered, ui->modelTreeView, &igQtModelListView::ChangeSelected2LastItem);
	//connect(ui->action_LastMesh, &QAction::triggered, rendererWidget, &igQtModelDrawWidget::changeCurrentModel2Last);

	//connect(ui->action_setViewTo_X, &QAction::triggered, rendererWidget, &igQtModelDrawWidget::UpdateCurrentViewToXP);
	//connect(ui->action_setViewTo_X1, &QAction::triggered, rendererWidget, &igQtModelDrawWidget::UpdateCurrentViewToXN);
	//connect(ui->action_setViewTo_Y, &QAction::triggered, rendererWidget, &igQtModelDrawWidget::UpdateCurrentViewToYP);
	//connect(ui->action_setViewTo_Y1, &QAction::triggered, rendererWidget, &igQtModelDrawWidget::UpdateCurrentViewToYN);
	//connect(ui->action_setViewTo_Z, &QAction::triggered, rendererWidget, &igQtModelDrawWidget::UpdateCurrentViewToZP);
	//connect(ui->action_setViewTo_Z1, &QAction::triggered, rendererWidget, &igQtModelDrawWidget::UpdateCurrentViewToZN);

	initAllMySignalConnections();
	initAllDockWidgetConnectWithAction();
}
igQtMainWindow::~igQtMainWindow() {
}

void igQtMainWindow::initAllFilters() {
	//connect(ui->action_LaSmoothing, &QAction::triggered, this, [&](bool checked) {
	//	iGame::iGameInformationMap* info = iGame::iGameInformationMap::New();
	//	info->Add("lambda", 0.05);
	//	info->Add("iterTime", 5);
	//	iGame::iGameManager::Instance()->ExecuteAlgorithm(iGame::iGameLaplacianSmoothing::New(), info);
	//	delete info;
	//	});
	//connect(ui->action_About, &QAction::triggered, this, [&](bool checked) {
	//	iGame::iGameInformationMap* info = iGame::iGameInformationMap::New();
	//	iGame::iGameManager::Instance()->ExecuteAlgorithm(iGame::iGameGenBaseScalar::New(), info);
	//	delete info;
	//	ui->widget_ScalarField->getScalarsName();
	//	});

	//connect(ui->action_Simplification, &QAction::triggered, this, [&](bool checked) {
	//	iGame::iGameInformationMap* info = iGame::iGameInformationMap::New();
	//	info->Add("TargetFaceNum", 0);
	//	info->Add("TargetReduction", 0.5);
	//	info->Add("NormalCheck", true);
	//	info->Add("OptimalPosition", true);
	//	info->Add("PreserveBoundary", true);
	//	info->Add("QualityCheck", true);
	//	iGame::iGameManager::Instance()->ExecuteAlgorithm(iGame::iGameQEMSimplification::New(), info);
	//	delete info;
	//	});


	//connect(ui->action_TetSimplification, &QAction::triggered, this, [&](bool checked) {
	//	iGame::iGameInformationMap* info = iGame::iGameInformationMap::New();
	//	iGame::iGameManager::Instance()->ExecuteAlgorithm(iGame::iGameTriDecimation::New(), info);
	//	delete info;
	//		});

	//connect(ui->action_Test, &QAction::triggered, this, [&](bool checked) {
	//	iGame::iGameInformationMap* info = iGame::iGameInformationMap::New();
	//	iGame::iGameManager::Instance()->ExecuteAlgorithm(iGame::iGameTest::New(), info);
	//	delete info;
	//	});
}

void igQtMainWindow::initAllDockWidgetConnectWithAction()
{
	//connect(ui->action_SearchInfo, &QAction::triggered, this, [&](bool checked) {
	//	ui->dockWidget_SearchInfo->show();
	//	});
	//connect(ui->action_IsShowColorBar, &QAction::triggered, this, &igQtMainWindow::updateColorBarShow);
	//connect(ui->action_ExportAnimation, &QAction::triggered, this, [&](bool checked) {
	//	ui->dockWidget_Animation->show();
	//	});
	//connect(ui->action_Scalar, &QAction::triggered, this, [&](bool checked) {
	//	ui->dockWidget_ScalarField->show();
	//	});
	//connect(ui->action_Vector, &QAction::triggered, this, [&](bool checked) {
	//	ui->dockWidget_VectorField->show();
	//	});
	//connect(ui->action_Tensor, &QAction::triggered, this, [&](bool checked) {
	//	ui->dockWidget_TensorField->show();
	//	});
	//connect(ui->action_FlowField, &QAction::triggered, this, [&](bool checked) {
	//	ui->dockWidget_FlowField->show();
	//	});
	//connect(ui->action_SearchInfo, &QAction::triggered, this, [&](bool checked) {
	//	ui->dockWidget_SearchInfo->show();
	//	});
	//connect(ui->action_EditMode, &QAction::triggered, this, [&](bool checked) {
	//	ui->dockWidget_EditMode->show();
	//	});
	//connect(ui->action_QualityDetection, &QAction::triggered, this, [&](bool checked) {
	//	ui->dockWidget_QualityDetection->show();
	//	});
}
void igQtMainWindow::initAllMySignalConnections()
{
	//connect(rendererWidget, &igQtModelDrawWidget::insertToModelListView, ui->modelTreeView, &igQtModelListView::InsertModel);

	connect(fileLoader, &igQtFileLoader::FinishReading, this, &igQtMainWindow::updateRecentFilePaths);
	connect(fileLoader, &igQtFileLoader::FinishReading, this, &igQtMainWindow::updateViewStyleAndCloudPicture);
	connect(fileLoader, &igQtFileLoader::FinishReading, this, &igQtMainWindow::updateCurrentSceneWidget);
    connect(fileLoader, &igQtFileLoader::EmitMakeCurrent, rendererWidget,&igQtRenderWidget::MakeCurrent);
    connect(fileLoader, &igQtFileLoader::EmitDoneCurrent, rendererWidget,&igQtRenderWidget::DoneCurrent);

	//connect(fileLoader, &igQtFileLoader::FinishReading, ui->widget_ScalarField, &igQtScalarViewWidget::getScalarsName);
	//connect(fileLoader, &igQtFileLoader::FinishReading, ui->widget_TensorField, [&]() {
	//	ui->widget_TensorField->UpdateTensorsNameList();
	//	});
	//connect(fileLoader, &igQtFileLoader::FinishReading, ui->widget_SearchInfo, &igQtSearchInfoWidget::updateDataProducer);

	connect(fileLoader, &igQtFileLoader::AddFileToModelList, ui->modelTreeView, &igQtModelListView::AddModel);
	//connect(fileLoader, &igQtFileLoader::LoadAnimationFile, ui->widget_Animation, &igQtAnimationWidget::initAnimationComponents);

	//connect(ui->widget_Animation, &igQtAnimationWidget::PlayAnimation_snap, rendererWidget, &igQtModelDrawWidget::PlayAnimation_snap);
	//connect(ui->widget_Animation, &igQtAnimationWidget::PlayAnimation_interpolate, rendererWidget, &igQtModelDrawWidget::PlayAnimation_interpolate);

	//connect(ui->widget_FlowField, &igQtStreamTracerWidget::sendstreams, rendererWidget, &igQtModelDrawWidget::DrawStreamline);
	//connect(ui->widget_FlowField, &igQtStreamTracerWidget::updatestreams, rendererWidget, &igQtModelDrawWidget::UpdateStreamline);

	connect(ui->modelTreeView, &igQtModelListView::UpdateCurrentScene, this, &igQtMainWindow::updateCurrentSceneWidget);
	connect(ui->modelTreeView, &igQtModelListView::UpdateCurrentItemToOtherQtModule, this, &igQtMainWindow::updateCurrentDataObject);

	//connect(ui->modelTreeView, &igQtModelListView::ChangeModelVisible, rendererWidget, &igQtModelDrawWidget::changeTargetModelVisible);
	//connect(ui->widget_ScalarField, &igQtScalarViewWidget::updateCurrentModelColor, rendererWidget, &igQtModelDrawWidget::UpdateCurrentModel);
	//connect(ui->widget_ScalarField, &igQtScalarViewWidget::changeColorBarShow, this, &igQtMainWindow::updateColorBarShow);
	//connect(ui->widget_QualityDetection, &igQtQualityDetectionWidget::updateCurrentModelColor, rendererWidget, &igQtModelDrawWidget::UpdateCurrentModel);
	//connect(ui->widget_ScalarField, &igQtScalarViewWidget::ChangeShowColorManager, this, [&]() {
	//	if (this->ColorManagerWidget->isHidden()) {
	//		this->ColorManagerWidget->resetColorRange();
	//		this->ColorManagerWidget->show();
	//	}
	//	else {
	//		this->ColorManagerWidget->hide();
	//	}
	//	});
	//connect(this->ColorManagerWidget, &igQtColorManagerWidget::UpdateColorBarFinished, this, [&]() {
	//	ui->widget_ScalarField->drawModelWithScalarData();
	//	this->rendererWidget->getColorBarWidget()->update();
	//	});
	//connect(ui->widget_EditMode, &igQtEditModeWidget::ChangeCutFlag, rendererWidget, &igQtModelDrawWidget::UpdateCutFlag);
	//connect(ui->widget_EditMode, &igQtEditModeWidget::UpdateCurrentModelCutPlane, rendererWidget, &igQtModelDrawWidget::UpdateCutPlane);
	//connect(ui->widget_EditMode, &igQtEditModeWidget::ChangeEditModeToModelView, rendererWidget, &igQtModelDrawWidget::UpdateEditModeToModelView);
	//connect(ui->widget_EditMode, &igQtEditModeWidget::ChangeEditModeToPickItem, rendererWidget, &igQtModelDrawWidget::UpdateEditModeToPickItem);
	//connect(this->rendererWidget, &igQtModelDrawWidget::updateSelectedFramePlane, this, [&]() {
	//	ui->widget_SearchInfo->searchDataWithFramePlane();
	//	});
	//connect(this->rendererWidget, &igQtModelDrawWidget::updatePickRay, this, [&](Vector3f p, Vector3f dir) {
	//	ui->widget_SearchInfo->searchDataWithRay(p, dir);
	//	});
	//connect(ui->widget_SearchInfo, &igQtSearchInfoWidget::showSearchedPoint, this, [&](iGameFloatArray* points) {
	//	this->rendererWidget->DrawSelectedPoint(points);
	//	});
	//connect(ui->widget_TensorField, &igQtTensorWidget::DrawEllipsoidGlyph, this, [&]() {
	//	this->rendererWidget->DrawEllipsoidGlyph();
	//	});
	//connect(ui->widget_TensorField, &igQtTensorWidget::UpdateEllipsoidGlyph, this, [&]() {
	//	this->rendererWidget->UpdateEllipsoidGlyph();
	//	});
}

void igQtMainWindow::updateRecentFilePaths()
{
	ui->menu_RecentFiles->clear();
	auto recentFileActions = fileLoader->GetRecentActionList();
	for (auto i = recentFileActions.size() - 1; i >= 0; i--) {
		ui->menu_RecentFiles->addAction(recentFileActions.at(i));
	}
}
void igQtMainWindow::updateColorBarShow()
{
	//auto colorBar = this->rendererWidget->getColorBarWidget();
	//if (colorBar->isHidden()) {
	//	colorBar->show();
	//}
	//else {
	//	colorBar->hide();
	//}
}
void igQtMainWindow::ChangeViewStyle()
{
	int item = viewStyleCombox->currentIndex();
	if (item < 0) return;
	this->rendererWidget->ChangeViewStyle(item);
}

void igQtMainWindow::ChangeScalarView()
{
	int item1 = attributeViewIndexCombox->currentIndex();
	int item2 = attributeViewDimCombox->currentIndex();
	if (item1 < 0) return;
	this->rendererWidget->ChangeScalarView(item1, item2 - 1);

	static const char* name[3] = { "x", "y", "z" };
	auto* current = rendererWidget->GetScene()->GetCurrentObject();
	attributeViewDimCombox->clear();
	attributeViewDimCombox->addItem("magnitude");
	int index = item1;
	if (index == 0) {
		attributeViewDimCombox->setCurrentIndex(0);
	}
	else {
		int num = current->GetPropertySet()->GetProperty(index - 1).pointer->GetElementSize();
		for (int i = 0; i < num; i++)
		{
			attributeViewDimCombox->addItem(name[i]);
		}
		attributeViewDimCombox->setCurrentIndex(current->GetAttributeDimension() + 1);
	}
}

void igQtMainWindow::ChangeScalarViewDim()
{
	int item1 = attributeViewIndexCombox->currentIndex();
	int item2 = attributeViewDimCombox->currentIndex();
	if (item1 < 0) return;
	if (item2 < 0) {
		this->rendererWidget->ChangeScalarView(item1, -1);
	}
	else {
		this->rendererWidget->ChangeScalarView(item1, item2 - 1);
	}
}
void igQtMainWindow::updateViewStyleAndCloudPicture()
{
	auto* current = rendererWidget->GetScene()->GetCurrentObject();
	if (current)
	{
		// Update View Style
		IGenum defaultViewStyle = current->GetViewStyle();
		viewStyleCombox->setCurrentIndex(defaultViewStyle);

		// Update Attribute View Index
		attributeViewIndexCombox->clear();
		attributeViewIndexCombox->addItem("None        ");

		StringArray::Pointer nameArray =
			current->GetMetadata()->GetStringArray(ATTRIBUTE_NAME_ARRAY);
		for (int i = 0; i < nameArray->Size(); i++)
		{
			attributeViewIndexCombox->addItem(QString::fromStdString(nameArray->GetElement(i)));
		}
		attributeViewIndexCombox->setCurrentIndex(current->GetAttributeIndex() + 1);
	}
}

void igQtMainWindow::updateCurrentDataObject()
{
	updateViewStyleAndCloudPicture();
	
}

void igQtMainWindow::updateCurrentSceneWidget() {
	this->rendererWidget->update();
}
