//
// Created by m_ky on 2024/5/22.
//

#include <IQComponents/igQtProgressBarWidget.h>
#include <QHBoxLayout>
//#include <iGameManager.h>
/**
 * @class   igQtProgressBarWidget
 * @brief   igQtProgressBarWidget's brief
 */
igQtProgressBarWidget::igQtProgressBarWidget(QWidget *parent) : QWidget(parent) {
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);

    progressBarLabel = new QLabel(DEFAULT,this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(progressBarLabel);
    layout->addWidget(progressBar);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    //this->ProgressObserver = iGame::iGameManager::Instance()->GetProgressObserver();
    //if (this->ProgressObserver)
    //{
    //    this->ProgressObserver->AddObserver(iGame::iGameCommand::BeginEvent,
    //        [&](iGame::iGameObject*, unsigned long, void* data)-> void {
    //            //const char* desc = static_cast<const char*>(data);
    //            this->updateProgressBarLabel("Process..");
    //            this->updateProgressBar(0);
    //        });
    //    this->ProgressObserver->AddObserver(iGame::iGameCommand::ProgressEvent, 
    //        [&](iGame::iGameObject*, unsigned long, void* data)-> void {
    //            double value = *static_cast<double*>(data);
    //            this->updateProgressBar(value);
    //            this->updateProgressBarLabel("Process..");
    //        });
    //    this->ProgressObserver->AddObserver(iGame::iGameCommand::EndEvent,
    //        [&](iGame::iGameObject*, unsigned long, void* data)-> void {
    //            this->updateProgressBarLabel(DEFAULT);
    //            this->updateProgressBar(0);
    //        });
    //}
}

void igQtProgressBarWidget::updateProgressBar(double value) {
    value = std::max(value, 0.0);
    value = std::min(value, 1.0);

    int progress = value * 100;
    progressBar->setValue(progress);
}

void igQtProgressBarWidget::updateProgressBarLabel(const char* info) {
    if (info == nullptr || info == "")
    {
        progressBarLabel->setText(DEFAULT);
    }
    else
    {
        progressBarLabel->setText(info);
    }
}
