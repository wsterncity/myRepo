//
// Created by m_ky on 2024/4/22.
//

#include <IQComponents/igQtAnimationTreeWidget_snap.h>
#include <IQComponents/igQtAnimationTreeWidget_interpolate.h>
#include <IQWidgets/igQtAnimationWidget.h>
#include <IQCore/igQtAnimationVcrController.h>
#include <QAbstractButton>

#include <qdebug.h>

#include <iGameFileIO.h>
#include <iGameSceneManager.h>

/**
 * @class   igQtAnimationWidget
 * @brief   igQtAnimationWidget's brief
 */
igQtAnimationWidget::igQtAnimationWidget(QWidget *parent) : QWidget(parent), ui(new Ui::Animation){
    ui->setupUi(this);
    VcrController = new igQtAnimationVcrController(this);

    connect(VcrController, &igQtAnimationVcrController::timeStepChanged_snap, this, &igQtAnimationWidget::playAnimation_snap);
    connect(VcrController, &igQtAnimationVcrController::timeStepChanged_interpolate, this, &igQtAnimationWidget::playAnimation_interpolate);
    connect(VcrController, &igQtAnimationVcrController::updateAnimationComponentsTimeStap, ui->treeWidget_snap, &igQtAnimationTreeWidget_snap::updateCurrentKeyframe);
    connect(VcrController, &igQtAnimationVcrController::updateAnimationComponentsTimeStap, ui->treeWidget_interpolate, &igQtAnimationTreeWidget_interpolate::updateCurrentKeyframe);
    connect(VcrController, &igQtAnimationVcrController::updateAnimationComponentsTimeStap, ui->SliderAnimationTrack, &QSlider::setValue);
    connect(VcrController, &igQtAnimationVcrController::finishPlaying, this, &igQtAnimationWidget::btnPlay_finishLoop);
    connect(ui->btnFirstFrame, &QPushButton::clicked, VcrController, &igQtAnimationVcrController::onFirstFrame);
    connect(ui->btnLastFrame, &QPushButton::clicked, VcrController, &igQtAnimationVcrController::onLastFrame);
    connect(ui->btnPreviousFrame, &QPushButton::clicked, VcrController, &igQtAnimationVcrController::onPreviousFrame);
    connect(ui->btnNextFrame, &QPushButton::clicked, VcrController, &igQtAnimationVcrController::onNextFrame);
    connect(ui->treeWidget_snap, &igQtAnimationTreeWidget_snap::keyframedChanged, VcrController, &igQtAnimationVcrController::updateCurrentKeyframe);
    connect(ui->treeWidget_interpolate, &igQtAnimationTreeWidget_interpolate::keyframedChanged, VcrController, &igQtAnimationVcrController::updateCurrentKeyframe);
    connect(ui->treeWidget_interpolate, &igQtAnimationTreeWidget_interpolate::updateVcrControllerInterpolateData, VcrController, &igQtAnimationVcrController::updateInterpolate);
    connect(ui->treeWidget_interpolate, &igQtAnimationTreeWidget_interpolate::updateComponentsKeyframeSum, this, &igQtAnimationWidget::updateAnimationComponentsKeyframeSum);
    connect(ui->rbtnSnapTimeMode, SIGNAL(toggled(bool)), this, SLOT(changeAnimationMode()));


    connect(ui->btnPlayOrPause, &QPushButton::toggled, this, [&](bool checked){
        if(checked){
            if(ui->btnReverseOrPause->isChecked())
            {
                ui->btnPlayOrPause->setChecked(false);
                ui->btnReverseOrPause->setChecked(false);
            }
            else {
                VcrController->onPlay(true);
                ui->btnPlayOrPause->setIcon(QIcon(":/Ticon/Icons/VcrPause.svg"));
            }
        }
        else {
            VcrController->onPause();
            ui->btnPlayOrPause->setIcon(QIcon(":/Ticon/Icons/VcrPlay.svg"));
        }
    });
    connect(ui->btnReverseOrPause, &QPushButton::toggled, this, [&](bool checked){
        if(checked){
            if(ui->btnPlayOrPause->isChecked()) ui->btnReverseOrPause->setChecked(false), ui->btnPlayOrPause->setChecked(false);
            else {
                VcrController->onPlay(false);
                ui->btnReverseOrPause->setIcon(QIcon(":/Ticon/Icons/VcrPause.svg"));
            }
        }
        else {
            VcrController->onPause();
            ui->btnReverseOrPause->setIcon(QIcon(":/Ticon/Icons/VcrReverse.svg"));
        }
    });
    connect(ui->btnLoop, &QPushButton::toggled, this, [&](bool checked){
        VcrController->onLoop(checked);
        if(checked){
            ui->btnLoop->setIcon(QIcon(":/Ticon/Icons/VcrLoop.svg"));
        }
        else {
            ui->btnLoop->setIcon(QIcon(":/Ticon/Icons/VcrDisabledLoop.svg"));
        }
    });
    connect(ui->btnApplyAnimationOperation, &QPushButton::clicked, this, [&](bool checked){
        VcrController->setStripe(ui->spinBoxAnimationStride->value());
        ui->treeWidget_interpolate->updateInterpolateData(
                ui->lineEditStartTime->text().toFloat(), ui->lineEditEndTime->text().toFloat() ,ui->lineEditKeyframeNum->text().toInt());
    });

    auto *validator = new QIntValidator(2, 999, this); // 限制关键帧输入范围为2到999，根据需要修改
    auto *LineValidator = new QRegExpValidator(QRegExp("^[0-9]*\\.?[0-9]*$"), this);
    ui->lineEditStartTime->setValidator(LineValidator);
    ui->lineEditEndTime->setValidator(LineValidator);
    ui->lineEditKeyframeNum->setValidator(validator);
//    ui->treeWidget_interpolate->header()->show();
    ui->treeWidget_interpolate->hide();




    //  Init the Animation Components if  model have the time value.
    std::vector<float> timevalue{1.0, 2.0, 3.0, 4.0};
    if(timevalue.empty() || timevalue.size() == 1) return;
    VcrController->initController(static_cast<int>(timevalue.size()), 1);
    ui->treeWidget_snap->initAnimationTreeWidget(timevalue);
    ui->treeWidget_interpolate->initAnimationTreeWidget(timevalue);
    ui->SliderAnimationTrack->setMaximum(static_cast<int>(timevalue.size()) - 1);
    ui->SliderAnimationTrack->setMinimum(0);
    ui->SliderAnimationTrack->setValue(0);

    ui->lineEditKeyframeNum->setText(QString("%1").arg(static_cast<int>(timevalue.size())));
    ui->lineEditStartTime->setText(QString::asprintf("%.f", *timevalue.begin()));
    ui->lineEditEndTime->setText(QString::asprintf("%.20f", *(timevalue.end() - 1)));
    connect(ui->SliderAnimationTrack, &QSlider::sliderMoved, VcrController, &igQtAnimationVcrController::updateCurrentKeyframe);

}

void igQtAnimationWidget::playAnimation_snap(int keyframe_idx){
    using namespace iGame;
    auto currentScene = SceneManager::Instance()->GetCurrentScene();
    auto currentObject = currentScene->GetCurrentObject();
    if(currentObject == nullptr || currentObject->GetTimeFrames()->GetArrays().empty())  return;
    std::cout << "current obj id : " << currentObject << '\n';
    auto& frameSubFiles = currentObject->GetTimeFrames()->GetTargetTimeFrame(keyframe_idx).SubFileNames;
    if(frameSubFiles->Size() > 1){
        currentObject->ClearSubDataObject();
        for(int i = 0; i < frameSubFiles->Size(); i ++){
            auto sub = FileIO::ReadFile(frameSubFiles->GetElement(i));
            currentObject->AddSubDataObject(sub);
        }
    }
    else if(frameSubFiles->Size() == 1){
        auto newDataObject = FileIO::ReadFile(frameSubFiles->GetElement(0));
        newDataObject->SetTimeFrames(currentObject->GetTimeFrames());
        currentScene->RemoveCurrentDataObject();
        currentScene->AddDataObject(newDataObject);
        newDataObject->SwitchToCurrentTimeframe(keyframe_idx);
    }
    Q_EMIT UpdateScene();
}

void igQtAnimationWidget::playAnimation_interpolate(int keyframe_0, float t){
    Q_EMIT PlayAnimation_interpolate(keyframe_0, t);
}

void igQtAnimationWidget::btnPlay_finishLoop() {
    ui->btnPlayOrPause->setChecked(false);
    ui->btnReverseOrPause->setChecked(false);
}

void igQtAnimationWidget::updateAnimationComponentsKeyframeSum(int keyframeSum) {
    VcrController->setKeyframe_sum(keyframeSum);
    ui->SliderAnimationTrack->setValue(0);
    ui->SliderAnimationTrack->setMaximum(keyframeSum - 1);
}

void igQtAnimationWidget::changeAnimationMode() {
    if(ui->rbtnSnapTimeMode->isChecked())
    {
        ui->treeWidget_interpolate->hide();
        ui->treeWidget_snap->show();
        updateAnimationComponentsKeyframeSum(ui->treeWidget_snap->getKeyframeSize());
        VcrController->setInterpolateMode(false);
    }else {
        ui->treeWidget_snap->hide();
        ui->treeWidget_interpolate->show();
        updateAnimationComponentsKeyframeSum(ui->treeWidget_interpolate->getKeyframeSize());
        VcrController->setInterpolateMode(true);
    }
}

void igQtAnimationWidget::initAnimationComponents() {
    if(iGame::SceneManager::Instance()->GetCurrentScene()->GetCurrentObject()->GetTimeFrames() == nullptr)  return;
    auto timeArrays = iGame::SceneManager::Instance()->GetCurrentScene()->GetCurrentObject()->GetTimeFrames()->GetArrays();
    if(timeArrays.empty()) return;
    std::vector<float> timeValues;
    timeValues.reserve(timeArrays.size());
    for(auto & timeArray : timeArrays) timeValues.push_back(timeArray.timeValue);
    VcrController->initController(static_cast<int>(timeValues.size()), 1);
    ui->treeWidget_snap->initAnimationTreeWidget(timeValues);
    ui->treeWidget_interpolate->initAnimationTreeWidget(timeValues);
    ui->SliderAnimationTrack->setMaximum(static_cast<int>(timeValues.size()) - 1);
    ui->SliderAnimationTrack->setMinimum(0);
    ui->SliderAnimationTrack->setValue(0);

    ui->lineEditKeyframeNum->setText(QString("%1").arg(static_cast<int>(timeValues.size())));
    ui->lineEditStartTime->setText(QString::asprintf("%.f", *timeValues.begin()));
    ui->lineEditEndTime->setText(QString::asprintf("%.20f", *(timeValues.end() - 1)));
    connect(ui->SliderAnimationTrack, &QSlider::sliderMoved, VcrController, &igQtAnimationVcrController::updateCurrentKeyframe);

}
