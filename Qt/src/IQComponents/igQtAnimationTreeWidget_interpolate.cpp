//
// Created by m_ky on 2024/3/19.
//

#include <IQComponents/igQtAnimationTreeWidget_interpolate.h>
#include <QDebug>
igQtAnimationTreeWidget_interpolate::igQtAnimationTreeWidget_interpolate(QWidget *parent) : igQtAnimationTreeWidget(parent) {
//    setHeaderLabels({"插值时间序列", "值"});
}


void igQtAnimationTreeWidget_interpolate::updateInterpolateSequence(int num) {
    interpolate_timeSequence.clear();
    interpolate_timeSequence.reserve(num);
    keyframe_sum = num;
    current_Keyframe_index = 0;
    float frameStep = (endTime- startTime) / (float)(keyframe_sum - 1);

    std::vector<std::pair<int, float>> interpolate_sequence;
    int start_keyframe_idx = 0;
    auto it = timeSequence.begin() + 1;
    while(it != timeSequence.end() && startTime > *it)
    {
        it ++;
        start_keyframe_idx ++;
    }
    interpolate_sequence.reserve(keyframe_sum);
    interpolate_timeSequence.reserve(keyframe_sum);
    for(int i = 0; i < keyframe_sum; i ++)
    {
        float t = startTime + frameStep * (float)i;
        while(t > *it)
        {
            it ++;
            start_keyframe_idx ++;
        }
        interpolate_sequence.emplace_back(start_keyframe_idx, (t - *(it - 1)) / (*it - *(it - 1)));
        interpolate_timeSequence.push_back(t);
    }
    this->topLevelItem(2)->setText(1, QString("%1").arg(keyframe_sum));
    removeAllChildItem(this->topLevelItem(3));

    for(int i = 0; i < keyframe_sum; i ++)
    {
        auto *item = new QTreeWidgetItem();
        item->setText(0, QString("%1").arg(i + 1));
        item->setText(1, QString::asprintf("%.10f", interpolate_timeSequence[i]));
        this->topLevelItem(3)->addChild(item);
    }
    this->topLevelItem(3)->setText(1, QString("[%1, %2]").arg(startTime).arg(endTime));
    Q_EMIT updateVcrControllerInterpolateData(interpolate_sequence);
    Q_EMIT updateComponentsKeyframeSum(num);
}

void igQtAnimationTreeWidget_interpolate::initAnimationTreeWidget(std::vector<float> &timeSeq) {
    current_Keyframe_index = 0;
    timeSequence = timeSeq;
    startTime = *timeSeq.begin();
    endTime = *(timeSeq.end() - 1);
    if(timeSeq.size() > 1){
        updateInterpolateSequence(static_cast<int>(timeSeq.size()));
        updateData();
    }
}

void igQtAnimationTreeWidget_interpolate::updateData() {
    this->topLevelItem(0)->setText(1, QString("%1").arg(current_Keyframe_index + 1));
    this->topLevelItem(1)->setText(1, QString::asprintf("%.10f", interpolate_timeSequence[current_Keyframe_index]));
}

void igQtAnimationTreeWidget_interpolate::updateInterpolateData(float _start, float _end, int keyframeNum) {
    if((startTime != _start || endTime != _end || keyframe_sum != keyframeNum) && startTime < endTime) {
        startTime = _start;
        endTime = _end;
        updateInterpolateSequence(keyframeNum);
    }
}

void igQtAnimationTreeWidget_interpolate::updateCurrentKeyframe(int idx) {
    if(idx >= keyframe_sum) return;
    current_Keyframe_index = idx;
    this->updateData();
}




