#include<IQWidgets/igQtStreamTracerWidget.h>
#include <iGameSceneManager.h>

igQtStreamTracerWidget::igQtStreamTracerWidget(QWidget* parent) : QWidget(parent), ui(new Ui::SteamLineTracer)
{
    ui->setupUi(this);
	connect(ui->control_comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeControl()));
	connect(ui->numOfSeedLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(changenumOfSeeds()));
	connect(ui->lengthOfStreamLine, SIGNAL(textChanged(const QString&)), this, SLOT(changelengthOfStreamLine()));
	connect(ui->lengthOfStep, SIGNAL(textChanged(const QString&)), this, SLOT(changelengthOfStep()));
	connect(ui->maxSteps, SIGNAL(textChanged(const QString&)), this, SLOT(changemaxSteps()));
	connect(ui->proportion_Slider, SIGNAL(valueChanged(int)), this, SLOT(changeProportion()));
	connect(ui->proportion_Slider, SIGNAL(sliderPressed()), this, SLOT(Pressed()));
	connect(ui->proportion_Slider, SIGNAL(sliderReleased()), this, SLOT(Released()));
	connect(ui->generate_streamline_btn, &QPushButton::clicked, this, &igQtStreamTracerWidget::generateStreamline);
	connect(ui->pushButton, &QPushButton::clicked, this, &igQtStreamTracerWidget::increaseProportion);
	connect(ui->pushButton_2, &QPushButton::clicked, this, &igQtStreamTracerWidget::reduceProportion);
	 numOfSeeds = 10;
	 control = 0;
	 proportion = 0.5;
	 lengthOfStreamLine = 5;
	 maxSteps = 2000;
	 lengthOfStep = 0.003;
	 terminalSpeed = 0.00001;
	 haveDraw = false;
	 haveClicked = true;
	ui->control_comboBox->setCurrentIndex(0);
	streamlineResult = UnstructuredMesh::New();
}
void igQtStreamTracerWidget::changeControl() {
	control=ui->control_comboBox->currentIndex();

    generateStreamline();
	//std::cout << "current index=" << control <<std::endl;
}
void igQtStreamTracerWidget::changenumOfSeeds() {
	numOfSeeds = ui->numOfSeedLineEdit->text().toInt();
	//std::cout << "current seeds=" << numOfSeeds << std::endl;
}
void igQtStreamTracerWidget::changelengthOfStreamLine() {
	lengthOfStreamLine = ui->lengthOfStreamLine->text().toFloat();
}
void igQtStreamTracerWidget::changelengthOfStep() {
	lengthOfStep = ui->lengthOfStep->text().toFloat();
}
void igQtStreamTracerWidget::changemaxSteps() {
	maxSteps = ui->maxSteps->text().toFloat();
}
void igQtStreamTracerWidget::changeterminalSpeed() {
	terminalSpeed = ui->terminalSpeed->text().toFloat();
}
void igQtStreamTracerWidget::Pressed() {
	haveClicked = false;
	std::cout << "aaa" << std::endl;
}
void igQtStreamTracerWidget::Released() {
	haveClicked = true;
	generateStreamline();
}
void igQtStreamTracerWidget::changeProportion() {
	proportion = ui->proportion_Slider->value();
	proportion /= 100;
	if(haveClicked)
    generateStreamline();
	//std::cout << "current value=" << proportion << std::endl;
}
void igQtStreamTracerWidget::reduceProportion() {
	proportion = ui->proportion_Slider->value();
	if (proportion) {
		proportion = proportion - 1;
		ui->proportion_Slider->setValue(proportion);
		//std::cout << "current value=" << proportion << std::endl;
	}
}
void igQtStreamTracerWidget::increaseProportion() {
	proportion = ui->proportion_Slider->value();
	if (proportion<99) {
		proportion = proportion + 1;
		ui->proportion_Slider->setValue(proportion);
	}
	//std::cout << "current value=" << proportion << std::endl;
}
void igQtStreamTracerWidget::generateStreamline() {
	
	auto scene = SceneManager::Instance()->GetCurrentScene();
	iGameStreamTracer streamtracer;
	Model::Pointer model = scene->GetCurrentModel();
	VolumeMesh::Pointer mesh = DynamicCast<VolumeMesh>(model->GetDataObject());
	streamtracer.SetMesh(mesh);
	auto seeds = streamtracer.streamSeedGenerate(control, proportion, numOfSeeds);
	//manager->GetCurrentModel()->seedPointsOfstreamline = new float[3 * seeds.size()];
	//for (size_t i = 0; i < seeds.size(); i++)
	//{
	//	manager->GetCurrentModel()->seedPointsOfstreamline[3 * i] = seeds[i][0];
	//	manager->GetCurrentModel()->seedPointsOfstreamline[3 * i + 1] = seeds[i][1];
	//	manager->GetCurrentModel()->seedPointsOfstreamline[3 * i + 2] = seeds[i][2];
	//}
	std::vector<std::vector<float>> streamlineColor;
	auto streamline = streamtracer.showStreamLineHex(seeds, "V", streamlineColor,lengthOfStreamLine, lengthOfStep,terminalSpeed,maxSteps);
	Points::Pointer points = Points::New();
	CellArray::Pointer cells = CellArray::New();
	FloatArray::Pointer colors = FloatArray::New();
	UnsignedIntArray::Pointer types = UnsignedIntArray::New();
	for (int i = 0; i < streamline.size(); i++) {
		IdArray::Pointer line = IdArray::New();
		for (int j = 0; j < streamline[i].size() / 3; j++) {
			int id = points->AddPoint(Point(streamline[i][j * 3], streamline[i][j * 3 + 1], streamline[i][j * 3 + 2]));
			colors->AddValue(streamlineColor[i][j * 3]);
			line->AddId(id);
		}
		cells->AddCellIds(line);
		types->AddValue(IG_POLY_LINE);
	}
	streamlineResult->SetPoints(points);
	streamlineResult->SetCells(cells, types);
	streamlineResult->GetAttributeSet()->AddAttribute(IG_SCALAR, IG_POINT, colors);


	//manager->GetCurrentModel()->streamline = streamline;
	if (!haveDraw) {
		haveDraw = true;
		emit NewModel(streamlineResult, ItemSource::Algorithm);
		scene->SetCurrentModel(model);
	}

}