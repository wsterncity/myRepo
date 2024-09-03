//
// Created by m_ky on 2024/4/10.
//

#include "Interactor/iGameFacesSelection.h"
#include "Interactor/iGamePointsSelection.h"
#include "Interactor/iGamePointPickedInteractor.h"
#include "Interactor/iGameLineSourceInteractor.h"
#include <iGameUnstructuredMesh.h>
#include <iGameFilterPoints.h>
#include <iGameDataSource.h>
#include <iGameSurfaceMeshFilterTest.h>
#include <iGameVolumeMeshFilterTest.h>
#include <iGameModelSurfaceFilters/iGameModelGeometryFilter.h>
#include <iGameSubdivision/iGameHexhedronSubdivision.h>
#include <iGameSubdivision/iGameQuadSubdivision.h>
#include <IQCore/igQtMainWindow.h>
#include <IQCore/igQtFileLoader.h>
#include <IQWidgets/igQtModelDrawWidget.h>

#include <IQComponents/igQtModelListView.h>
#include <IQWidgets/ColorManager/igQtBasicColorAreaWidget.h>
#include <IQWidgets/ColorManager/igQtColorManagerWidget.h>
#include <IQComponents/igQtFilterDialogDockWidget.h>
#include <IQComponents/igQtProgressBarWidget.h>
#include <IQComponents/igQtModelDialogWidget.h>
#include <IQWidgets/igQtTensorWidget.h>
#include "iGameFileIO.h"

#include <Sources/iGameLineTypePointsSource.h>

igQtMainWindow::igQtMainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	modelTreeWidget = new igQtModelDialogWidget(this);
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
	ui->dockWidget_ModelList->hide();
	ui->dockWidget_ModelList->setWidget(ui->modelTreeView);
	// Setup default GUI layout.
	this->setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);
	this->setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);
	this->setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::North);
	// Set up the dock window corners to give the vertical docks more room.
	this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	this->addDockWidget(Qt::LeftDockWidgetArea, modelTreeWidget);

	initToolbarComponent();
	initAllComponents();
	initAllFilters();
	initAllSources();
	updateRecentFilePaths();

	connect(ui->action_select_point, &QAction::triggered, this,
		&igQtMainWindow::changePointSelectionInteractor);
	connect(ui->action_select_points, &QAction::triggered, this,
		&igQtMainWindow::changePointsSelectionInteractor);

	connect(ui->action_select_face, &QAction::triggered, this,
		&igQtMainWindow::changeFaceSelectionInteractor);
	connect(ui->action_select_faces, &QAction::triggered, this,
		&igQtMainWindow::changeFacesSelectionInteractor);


}
void igQtMainWindow::initToolbarComponent()
{
	//viewStyleCombox = new QComboBox(this);
	//viewStyleCombox->addItem("Points");
	//viewStyleCombox->addItem("WireFrame");
	//viewStyleCombox->addItem("Surface");
	//viewStyleCombox->addItem("Surface With Edegs");
	//viewStyleCombox->addItem("Volume");
	//viewStyleCombox->addItem("Volume With Edegs");

	//viewStyleCombox->setStyleSheet("QComboBox {"
	//	"background-color: #f0f0f0;"
	//	"color: #202020;"              // 设置文本颜色为浅白色
	//	"border: 1px solid #ffffff;"   // 设置边框样式为灰色实线边框
	//	"padding: 5px;"                // 设置内边距
	//	"font-size: 16px;"              // 设置下拉菜单项字体大小为14px
	//	"}"
	//	"QComboBox QAbstractItemView {"
	//	"font-family: Arial;"           // 设置下拉菜单项字体为Arial
	//	"color: #404040;"               // 设置下拉菜单项字体颜色为浅灰色
	//	"}"
	//	"QComboBox::drop-down {"
	//	"subcontrol-origin: padding;"
	//	"subcontrol-position: top right;"
	//	"width: 20px;"
	//	"border-left: 1px solid #202020;"
	//	"border-color: #eeeeee;"
	//	"}"
	//);

	//connect(viewStyleCombox, SIGNAL(currentIndexChanged(QString)), this, SLOT(ChangeViewStyle()));
	//ui->toolBar_meshview->addWidget(viewStyleCombox);

	//attributeViewIndexCombox = new QComboBox(this);
	//attributeViewIndexCombox->addItem("None        ");
	//attributeViewIndexCombox->setStyleSheet("QComboBox {"
	//	"background-color: #f0f0f0;"
	//	"color: #202020;"              // 设置文本颜色为浅白色
	//	"border: 1px solid #ffffff;"   // 设置边框样式为灰色实线边框
	//	"padding: 5px;"                // 设置内边距
	//	"font-size: 16px;"              // 设置下拉菜单项字体大小为14px
	//	"}"
	//	"QComboBox QAbstractItemView {"
	//	"font-family: Arial;"           // 设置下拉菜单项字体为Arial
	//	"color: #404040;"               // 设置下拉菜单项字体颜色为浅灰色
	//	"}"
	//	"QComboBox::drop-down {"
	//	"subcontrol-origin: padding;"
	//	"subcontrol-position: top right;"
	//	"width: 20px;"
	//	"border-left: 1px solid #202020;"
	//	"border-color: #eeeeee;"
	//	"}"
	//);

	//connect(attributeViewIndexCombox, SIGNAL(activated(int)), this, SLOT(ChangeScalarView()));
	//ui->toolBar_attribute_view_index->addWidget(attributeViewIndexCombox);


	//attributeViewDimCombox = new QComboBox(this);
	//attributeViewDimCombox->addItem("magnitude");
	//attributeViewDimCombox->setStyleSheet("QComboBox {"
	//	"background-color: #f0f0f0;"
	//	"color: #202020;"              // 设置文本颜色为浅白色
	//	"border: 1px solid #ffffff;"   // 设置边框样式为灰色实线边框
	//	"padding: 5px;"                // 设置内边距
	//	"font-size: 16px;"              // 设置下拉菜单项字体大小为14px
	//	"}"
	//	"QComboBox QAbstractItemView {"
	//	"font-family: Arial;"           // 设置下拉菜单项字体为Arial
	//	"color: #404040;"               // 设置下拉菜单项字体颜色为浅灰色
	//	"}"
	//	"QComboBox::drop-down {"
	//	"subcontrol-origin: padding;"
	//	"subcontrol-position: top right;"
	//	"width: 10px;"
	//	"border-left: 1px solid #202020;"
	//	"border-color: #eeeeee;"
	//	"}"
	//);

	//connect(attributeViewDimCombox, SIGNAL(activated(int)), this, SLOT(ChangeScalarViewDim()));
	//ui->toolBar_attribute_view_dim->addWidget(attributeViewDimCombox);
}
void igQtMainWindow::initAllComponents()
{

	// init ProgressBar
	progressBarWidget = new igQtProgressBarWidget(this);
	this->statusBar()->addPermanentWidget(progressBarWidget);

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
	connect(ui->action_ResetCenter, &QAction::triggered, this, [&]() {
		SceneManager::Instance()->GetCurrentScene()->ResetCenter();
		std::cout << "dsadas" << '\n';
		rendererWidget->update();
		});
	//connect(ui->action_PickCenter, &QAction::triggered, this, [&]() {
	//	float x = -1.0, y = -1.0, z = -1.0;
	//	iGame::iGameManager::Instance()->UpdateCenter(x, y, z);
	//	rendererWidget->update();
	//	});
	connect(ui->action_DeleteMesh, &QAction::triggered, ui->modelTreeView, &igQtModelListView::DeleteCurrentFile);
	//connect(ui->action_DeleteMesh, &QAction::triggered, this, &igQtMainWindow::updateCurrentSceneWidget);
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
	connect(ui->action_test_01, &QAction::triggered, this, [&](bool checked) {
		PointSet::Pointer points = PointSet::New();
		Points::Pointer ps = Points::New();

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, 1.0);

		double r = 1;
		for (int i = 0; i < 1000000; i++) {
			double u = dis(gen);
			double v = dis(gen);
			double w = dis(gen);

			double theta = 2.0 * M_PI * u;
			double phi = acos(2.0 * v - 1.0);
			double rCubeRoot = std::cbrt(w);

			double x = r * rCubeRoot * sin(phi) * cos(theta);
			double y = r * rCubeRoot * sin(phi) * sin(theta);
			double z = r * rCubeRoot * cos(phi);

			ps->AddPoint(x, y, z);
		}
		points->SetPoints(ps);
		points->SetName("undefined_PointSet");
		rendererWidget->AddDataObject(points);
		});

	connect(ui->action_test_02, &QAction::triggered, this, [&](bool checked) {
		FilterPoints::Pointer fp = FilterPoints::New();
		fp->SetInput(rendererWidget->GetScene()->GetCurrentModel()->GetDataObject());
		fp->SetFilterRate(0.5);
		fp->Execute();
		rendererWidget->update();
		});

	connect(ui->action_test_03, &QAction::triggered, this, [&](bool checked) {
		SurfaceMesh::Pointer mesh = SurfaceMesh::New();
		Points::Pointer points = Points::New();
		points->AddPoint(0, 0, 0);
		points->AddPoint(1, 0, 0);
		points->AddPoint(0, 1, 0);

		CellArray::Pointer faces = CellArray::New();
		faces->AddCellId3(0, 1, 2);

		mesh->SetPoints(points);
		mesh->SetFaces(faces);
		mesh->SetName("undefined_mesh");
		rendererWidget->AddDataObject(mesh);
		});

	connect(ui->action_test_04, &QAction::triggered, this, [&](bool checked) {
		SurfaceMeshFilterTest::Pointer fp = SurfaceMeshFilterTest::New();
		fp->SetInput(rendererWidget->GetScene()->GetCurrentModel()->GetDataObject());
		fp->Execute();
		rendererWidget->update();
		});

	connect(ui->action_test_05, &QAction::triggered, this, [&](bool checked) {
		//VolumeMeshFilterTest::Pointer fp = VolumeMeshFilterTest::New();
		//fp->SetInput(rendererWidget->GetScene()->GetCurrentModel()->GetDataObject());
		//fp->Execute();
		//rendererWidget->update();

		igQtFilterDialogDockWidget* dialog = new igQtFilterDialogDockWidget(this);
		int targetId = dialog->addParameter(igQtFilterDialogDockWidget::QT_LINE_EDIT, "Target number of faces", "1000");
		int reductionId = dialog->addParameter(igQtFilterDialogDockWidget::QT_LINE_EDIT, "Reduction (0..1)", "0");
		int thresholdId = dialog->addParameter(igQtFilterDialogDockWidget::QT_LINE_EDIT, "Quality threshold", "0.1");
		int preserveId = dialog->addParameter(igQtFilterDialogDockWidget::QT_CHECK_BOX, "Preserve Boundary of the mesh", "false");
		dialog->show();

		bool ok;
		std::cout << dialog->getInt(targetId, ok) << std::endl;
		std::cout << dialog->getDouble(reductionId, ok) << std::endl;
		std::cout << dialog->getDouble(thresholdId, ok) << std::endl;
		std::cout << (dialog->getChecked(preserveId, ok) ? "true" : "false") << std::endl;

		dialog->setApplyFunctor([&]() {
			std::cout << "123\n";
			});


		});

	//connect(ui->action_test_05, &QAction::triggered, this, [&](bool checked) {
	//	VolumeMeshFilterTest::Pointer fp = VolumeMeshFilterTest::New();
	//	fp->SetInput(rendererWidget->GetScene()->GetCurrentObject());
	//	fp->Execute();
	//	rendererWidget->update();
	//	});

	connect(ui->action_test_06, &QAction::triggered, this, [&](bool checked) {
		//      LineSource::Pointer source = LineSource::New();
		//      Points::Pointer points = Points::New();
		//      FloatArray::Pointer colors = FloatArray::New();
		//      CellArray::Pointer polylines = CellArray::New();
		//      CellArray::Pointer lines = CellArray::New();
		//      IdArray::Pointer pl = IdArray::New();
		//      for (int i = 0; i < 10; ++i) {
		//          float theta = 2 * M_PI * i / 10;
		//          float x = std::cos(theta);
		//          float y = std::sin(theta);
		//          IGsize ptId = points->AddPoint(Point{ x, y, 0.0f});
		//          colors->AddValue((x + y) / 2);
		//          pl->AddId(ptId);
		//      }
		//      polylines->AddCellIds(pl);

		//IGsize ptId0 = points->A
		// ddPoint(Point{0, 0, 0});
		//IGsize ptId1 = points->AddPoint(Point{0, 0, 1});
		//      colors->AddValue(0.3);
		//      colors->AddValue(0.4);
		//      lines->AddCellId2(ptId0, ptId1);

		//      source->SetPoints(points);
		//      source->SetColors(colors);
		//      source->SetLines(lines);
		//      source->SetPolyLines(polylines);
		//      source->SetName("undefined_line_source");
		//      rendererWidget->AddDataObject(source);

		UnstructuredMesh::Pointer mesh = UnstructuredMesh::New();
		Points::Pointer points = Points::New();
		CellArray::Pointer cells = CellArray::New();
		UnsignedIntArray::Pointer types = UnsignedIntArray::New();
		FloatArray::Pointer property = FloatArray::New();
		property->SetName("scalar");

		points->AddPoint(-0.5, -0.5, 0);
		points->AddPoint(0.5, -0.5, 0);
		points->AddPoint(0, 0.5, 0);
		points->AddPoint(0, 0, 0.6);

		points->AddPoint(1, 1, 0);
		points->AddPoint(2, 1, 0);
		points->AddPoint(1, 2, 0);

		property->AddValue(-0.5);
		property->AddValue(0.5);
		property->AddValue(0);
		property->AddValue(0);
		property->AddValue(1);
		property->AddValue(2);
		property->AddValue(1);

		cells->AddCellId4(0, 1, 2, 3);
		types->AddValue(IG_TETRA);

		cells->AddCellId3(4, 5, 6);
		types->AddValue(IG_TRIANGLE);

		StringArray::Pointer sa = StringArray::New();
		sa->AddElement("scalar");

		mesh->GetMetadata()->AddStringArray(ATTRIBUTE_NAME_ARRAY, sa);
		mesh->GetAttributeSet()->AddScalar(IG_POINT, property);
		mesh->SetPoints(points);
		mesh->SetCells(cells, types);
		mesh->SetName("undefined_unstructured_mesh");

		SceneManager::Instance()->GetCurrentScene()->CreateModel(mesh);
		modelTreeWidget->addDataObjectToModelTree(mesh, ItemSource::File);
		//this->updateCurrentDataObject();
		});
	auto action = ui->menuTest->addAction("surfaceExtractTest");

	connect(action, &QAction::triggered, this,
		[&](bool checked) {
			auto fp = iGameModelGeometryFilter::New();
			auto input = rendererWidget->GetScene()->GetCurrentModel()->GetDataObject();
			fp->Execute(input);
			SceneManager::Instance()->GetCurrentScene()->ChangeModelVisibility(
				0, false);
			auto mesh = fp->GetOutPut();
			rendererWidget->AddDataObject(mesh);
		});

	auto action_subdivision = ui->menuTest->addAction("Subdivision");

	connect(action_subdivision, &QAction::triggered, this,
		[&](bool checked) {
			//auto filter = HexhedronSubdivision::New();
			//VolumeMesh::Pointer mesh = DynamicCast<VolumeMesh>(rendererWidget->GetScene()->GetCurrentModel()->GetDataObject());
			auto filter =QuadSubdivision::New();
			SurfaceMesh::Pointer mesh = DynamicCast<SurfaceMesh>(rendererWidget->GetScene()->GetCurrentModel()->GetDataObject());
			filter->SetMesh(mesh);
			filter->Execute();
			auto ControlPoints = filter->GetOutput();
			ControlPoints->SetName("ControlPoints");

			modelTreeWidget->addDataObjectToModelTree(ControlPoints, ItemSource::File);

		});
}

void igQtMainWindow::initAllDockWidgetConnectWithAction()
{
	//connect(ui->action_SearchInfo, &QAction::triggered, this, [&](bool checked) {
	//	ui->dockWidget_SearchInfo->sh
	// ow();
	//	});
	//connect(ui->action_IsShowColorBar, &QAction::triggered, this, &igQtMainWindow::updateColorBarShow);
	connect(ui->action_ExportAnimation, &QAction::triggered, this, [&](bool checked) {
		ui->dockWidget_Animation->show();
		});
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

	connect(fileLoader, &igQtFileLoader::NewModel, modelTreeWidget, &igQtModelDialogWidget::addDataObjectToModelTree);
//	connect(fileLoader, &igQtFileLoader::FinishReading, this, &igQtMainWindow::updateRecentFilePaths);
	//connect(fileLoader, &igQtFileLoader::FinishReading, this, &igQtMainWindow::updateViewStyleAndCloudPicture);
	//connect(fileLoader, &igQtFileLoader::FinishReading, this, &igQtMainWindow::updateCurrentSceneWidget);
	connect(fileLoader, &igQtFileLoader::FinishReading, ui->widget_Animation, &igQtAnimationWidget::initAnimationComponents);

	//connect(fileLoader, &igQtFileLoader::FinishReading, ui->widget_ScalarField, &igQtScalarViewWidget::getScalarsName);
	//connect(fileLoader, &igQtFileLoader::FinishReading, ui->widget_TensorField, [&]() {
	//	ui->widget_TensorField->UpdateTensorsNameList();
	//	});
	//connect(fileLoader, &igQtFileLoader::FinishReading, ui->widget_SearchInfo, &igQtSearchInfoWidget::updateDataProducer);

	connect(fileLoader, &igQtFileLoader::AddFileToModelList, ui->modelTreeView, &igQtModelListView::AddModel);
	//connect(rendererWidget, &igQtRenderWidget::AddDataObjectToModelList, ui->modelTreeView, &igQtModelListView::AddModel);
	//connect(rendererWidget, &igQtRenderWidget::UpdateCurrentDataObject, this, &igQtMainWindow::updateCurrentDataObject);

	//connect(fileLoader, &igQtFileLoader::LoadAnimationFile, ui->widget_Animation, &igQtAnimationWidget::initAnimationComponents);

	connect(ui->widget_Animation, &igQtAnimationWidget::UpdateScene, this, &igQtMainWindow::UpdateRenderingWidget);


	//connect(ui->widget_Animation, &igQtAnimationWidget::PlayAnimation_interpolate, rendererWidget, &igQtModelDrawWidget::PlayAnimation_interpolate);
//	connect(ui->widget_Animation, &igQtAnimationWidget::PlayAnimation_snap, this, [&](int keyframe){
//		using namespace iGame;
//		auto currentObject = SceneManager::Instance()->GetCurrentScene()->GetCurrentObject();
//		if(currentObject == nullptr || currentObject->GetTimeFrames()->GetArrays().empty())  return;
//		auto& frameSubFiles = currentObject->GetTimeFrames()->GetTargetTimeFrame(keyframe).SubFileNames;
//		rendererWidget->makeCurrent();
//		if(frameSubFiles->Size() > 1){
//			currentObject->ClearSubDataObject();
//			for(int i = 0; i < frameSubFiles->Size(); i ++){
//				DataObject::Pointer sub = FileIO::ReadFile(frameSubFiles->GetElement(i));
//				currentObject->AddSubDataObject(sub);
//			}
//		}
//		else {
//			SceneManager::Instance()->GetCurrentScene()->RemoveCurrentDataObject();
//			currentObject = FileIO::ReadFile(frameSubFiles->GetElement(0));
//			currentObject->SetTimeFrames(currentObject->GetTimeFrames());
//			SceneManager::Instance()->GetCurrentScene()->AddDataObject(currentObject);
//		}
//		currentObject->SwitchToCurrentTimeframe(keyframe);
//		rendererWidget->doneCurrent();
//	});

	//connect(ui->widget_FlowField, &igQtStreamTracerWidget::sendstreams, rendererWidget, &igQtModelDrawWidget::DrawStreamline);
	//connect(ui->widget_FlowField, &igQtStreamTracerWidget::updatestreams, rendererWidget, &igQtModelDrawWidget::UpdateStreamline);

	//connect(ui->modelTreeView, &igQtModelListView::UpdateCurrentScene, this, &igQtMainWindow::updateCurrentSceneWidget);
	//connect(ui->modelTreeView, &igQtModelListView::UpdateCurrentItemToOtherQtModule, this, &igQtMainWindow::updateCurrentDataObject);

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
//void igQtMainWindow::ChangeViewStyle()
//{
//	int item = viewStyleCombox->currentIndex();
//	if (item < 0) return;
//	this->rendererWidget->ChangeViewStyle(item);
//}
//
//void igQtMainWindow::ChangeScalarView()
//{
//	int item1 = attributeViewIndexCombox->currentIndex();
//	int item2 = attributeViewDimCombox->currentIndex();
//	if (item1 < 0) return;
//	this->rendererWidget->ChangeScalarView(item1, item2 - 1);
//
//	static const char* name[3] = { "x", "y", "z" };
//	auto* current = rendererWidget->GetScene()->GetCurrentObject();
//	attributeViewDimCombox->clear();
//	attributeViewDimCombox->addItem("magnitude");
//	int index = item1;
//	if (index == 0) {
//		attributeViewDimCombox->setCurrentIndex(0);
//	}
//	else {
//		int num = current->GetPropertySet()->GetProperty(index - 1).pointer->GetElementSize();
//		for (int i = 0; i < num; i++)
//		{
//			attributeViewDimCombox->addItem(name[i]);
//		}
//		attributeViewDimCombox->setCurrentIndex(current->GetAttributeDimension() + 1);
//	}
//}
//
//void igQtMainWindow::ChangeScalarViewDim()
//{
//	int item1 = attributeViewIndexCombox->currentIndex();
//	int item2 = attributeViewDimCombox->currentIndex();
//	if (item1 < 0) return;
//	if (item2 < 0) {
//		this->rendererWidget->ChangeScalarView(item1, -1);
//	}
//	else {
//		this->rendererWidget->ChangeScalarView(item1, item2 - 1);
//	}
//}
//void igQtMainWindow::updateViewStyleAndCloudPicture()
//{
//	auto* current = rendererWidget->GetScene()->GetCurrentObject();
//	if (current)
//	{
//		// Update View Style
//		IGenum defaultViewStyle = current->GetViewStyle();
//		viewStyleCombox->setCurrentIndex(defaultViewStyle);
//
//		// Update Attribute View Index
//		attributeViewIndexCombox->clear();
//		attributeViewIndexCombox->addItem("None        ");
//
//		StringArray::Pointer nameArray =
//			current->GetMetadata()->GetStringArray(ATTRIBUTE_NAME_ARRAY);
//		if (nameArray != nullptr)
//		{
//			for (int i = 0; i < nameArray->Size(); i++)
//			{
//				attributeViewIndexCombox->addItem(QString::fromStdString(nameArray->GetElement(i)));
//			}
//		}
//		attributeViewIndexCombox->setCurrentIndex(current->GetAttributeIndex() + 1);
//	}
//}
//
//void igQtMainWindow::updateCurrentDataObject()
//{
//	updateViewStyleAndCloudPicture();
//	
//}
//
//void igQtMainWindow::updateCurrentSceneWidget() {
//	this->rendererWidget->update();
//}

void igQtMainWindow::changePointSelectionInteractor()
{
	if (ui->action_select_point->isChecked())
	{
		auto interactor = PointPickedInteractor::New();
		interactor->SetPointSet(DynamicCast<PointSet>(
			rendererWidget->GetScene()->GetCurrentModel()->GetDataObject()), rendererWidget->GetScene()->GetCurrentModel());
		rendererWidget->ChangeInteractor(interactor);

		if (ui->action_select_points->isChecked()) {
			ui->action_select_points->setChecked(false);
		}
	}
	else {
		rendererWidget->ChangeInteractor(Interactor::New());
	}
}

void igQtMainWindow::changePointsSelectionInteractor()
{
	if (ui->action_select_points->isChecked())
	{
		auto interactor = PointsSelection::New();
		interactor->SetPointSet(DynamicCast<PointSet>(
			rendererWidget->GetScene()->GetCurrentModel()->GetDataObject()), rendererWidget->GetScene()->GetCurrentModel());
		rendererWidget->ChangeInteractor(interactor);

		if (ui->action_select_point->isChecked()) {
			ui->action_select_point->setChecked(false);
		}
	}
	else {
		rendererWidget->ChangeInteractor(Interactor::New());
	}
}

void igQtMainWindow::changeFaceSelectionInteractor()
{

}

void igQtMainWindow::changeFacesSelectionInteractor()
{
	if (ui->action_select_faces->isChecked())
	{
		auto interactor = FacesSelection::New();
		interactor->SetModel(rendererWidget->GetScene()->GetCurrentModel());
		rendererWidget->ChangeInteractor(interactor);

		if (ui->action_select_face->isChecked()) {
			ui->action_select_face->setChecked(false);
		}
	}
	else {
		rendererWidget->ChangeInteractor(Interactor::New());
	}
}

void igQtMainWindow::initAllSources() {
    connect(ui->action_LineSource, &QAction::triggered, this, [&](){
        UnstructuredMesh::Pointer newLinePointSet = UnstructuredMesh::New();
        newLinePointSet->SetViewStyle(IG_POINTS);
        newLinePointSet->AddPoint(Point(0.f, 0.f, 0.f));
        newLinePointSet->AddPoint(Point(1.f, 1.0f, 1.f));
        igIndex cell[1] = {0};
        newLinePointSet->AddCell(cell, 1, IG_VERTEX);
        cell[0] = 1;
        newLinePointSet->AddCell(cell, 1, IG_VERTEX);
        auto curScene = SceneManager::Instance()->GetCurrentScene();

        LineTypePointsSource::Pointer lineSource = LineTypePointsSource::New();

        lineSource->SetInput(newLinePointSet);
        lineSource->SetResolution(20);
        lineSource->GetOutput()->SetName("lineSource");

        auto model = curScene->CreateModel(lineSource->GetOutput());
        modelTreeWidget->addModelToModelTree(model);
        auto interactor = LineSourceInteractor::New();

//        auto interactor = PointDragInteractor::New();
        interactor->SetPointSet(DynamicCast<PointSet>(SceneManager::Instance()->GetCurrentScene()->GetCurrentModel()->GetDataObject()));

        rendererWidget->ChangeInteractor(interactor);
    });
}

void igQtMainWindow::UpdateRenderingWidget() {
    rendererWidget->update();
}
