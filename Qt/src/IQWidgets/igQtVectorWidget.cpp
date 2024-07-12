#include<IQWidgets/igQtVectorWidget.h>
igQtVectorWidget::igQtVectorWidget(QWidget* parent) : QWidget(parent), ui(new Ui::igVector) {
    ui->setupUi(this);
	connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(changeHR()));
	connect(ui->horizontalSlider_2, SIGNAL(valueChanged(int)), this, SLOT(changeHL()));
	connect(ui->horizontalSlider_3, SIGNAL(valueChanged(int)), this, SLOT(changeTR()));
	connect(ui->horizontalSlider_4, SIGNAL(valueChanged(int)), this, SLOT(changeTL()));
}
void igQtVectorWidget::changeHR() {
	headRadius = ui->horizontalSlider->value();
	headRadius /= 100;
	std::cout << "current value=" << headRadius << std::endl;
}
void igQtVectorWidget::changeHL() {
	headLength = ui->horizontalSlider_2->value();
	headLength /= 100;
	std::cout << "current value=" << headLength << std::endl;
}
void igQtVectorWidget::changeTR() {
	tailRadius = ui->horizontalSlider_3->value();
	tailRadius /= 100;
	std::cout << "current value=" << tailRadius << std::endl;
}
void igQtVectorWidget::changeTL() {
	tailLength = ui->horizontalSlider_4 ->value();
	tailLength /= 100;
	std::cout << "current value=" << tailLength << std::endl;
}