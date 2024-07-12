#include<IQWidgets/igQtStreamTracerWidget.h>

igQtStreamTracerWidget::igQtStreamTracerWidget(QWidget* parent) : QWidget(parent), ui(new Ui::SteamLineTracer)
{
    ui->setupUi(this);
	connect(ui->control_comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeControl()));
	connect(ui->numOfSeedLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(changenumOfSeeds()));
	connect(ui->lengthOfStreamLine, SIGNAL(textChanged(const QString&)), this, SLOT(changelengthOfStreamLine()));
	connect(ui->lengthOfStep, SIGNAL(textChanged(const QString&)), this, SLOT(changelengthOfStep()));
	connect(ui->proportion_Slider, SIGNAL(valueChanged(int)), this, SLOT(changeProportion()));
	connect(ui->proportion_Slider, SIGNAL(sliderPressed()), this, SLOT(Pressed()));
	connect(ui->proportion_Slider, SIGNAL(sliderReleased()), this, SLOT(Released()));
	connect(ui->generate_streamline_btn, &QPushButton::clicked, this, &igQtStreamTracerWidget::generateStreamline);
	connect(ui->pushButton, &QPushButton::clicked, this, &igQtStreamTracerWidget::increaseProportion);
	connect(ui->pushButton_2, &QPushButton::clicked, this, &igQtStreamTracerWidget::reduceProportion);
	 numOfSeeds = 0;
	 control = 0;
	 proportion = 0;
	 lengthOfStreamLine = 0;
	 lengthOfStep = 0.003;
	 terminalSpeed = 0.00001;
	 haveDraw = false;
	 haveClicked = true;
	ui->control_comboBox->setCurrentIndex(0);
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
void igQtStreamTracerWidget::changeterminalSpeed() {
	terminalSpeed = ui->terminalSpeed->text().toFloat();
}
void igQtStreamTracerWidget::Pressed() {
	haveClicked = false;
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
	//auto manager = iGame::iGameManager::Instance();
 //   auto hexMesh = dynamic_cast<iGame::iGameHexMesh* >(manager->GetCurrentModel()->Mesh);
	//hexMesh->BuildFaces();
	////hexMesh->BuildEdges();
	////hexMesh->BuildEdgeToVolumeLink();
	//hexMesh->BuildFaceToVolumeLink();
	//hexMesh->BuildVolumeToVolumeLink();

	//iGameStreamTracer streamtracer;
	////auto seeds = streamtracer.seedTest(manager->GetCurrentModel());
	//auto seeds = streamtracer.streamSeedGenerate(manager->GetCurrentModel(),control, proportion, numOfSeeds);
	//manager->GetCurrentModel()->seedPointsOfstreamline = new float[3 * seeds.size()];
	//for (size_t i = 0; i < seeds.size(); i++)
	//{
	//	manager->GetCurrentModel()->seedPointsOfstreamline[3 * i] = seeds[i][0];
	//	manager->GetCurrentModel()->seedPointsOfstreamline[3 * i + 1] = seeds[i][1];
	//	manager->GetCurrentModel()->seedPointsOfstreamline[3 * i + 2] = seeds[i][2];
	//}
	//auto streamline = streamtracer.showStreamLine(seeds, manager->GetCurrentModel(), "V", manager->GetCurrentModel()->streamlineColor,lengthOfStreamLine, lengthOfStep,terminalSpeed);
	//manager->GetCurrentModel()->streamline = streamline;
	//if (!haveDraw) {
	//	haveDraw = true;
	//	Q_EMIT sendstreams();
	//}
	//else {
	//	Q_EMIT updatestreams();
	//}
}