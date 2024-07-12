#include "IQWidgets/ColorManager/igQtColorRangeSlider.h"
#include <QDebug>
//#include <iGameManager.h>
//#include <iGameColorMap.h>
//#include <iGameModelColorManager.h>
namespace
{

    const int SliderBarHeight = 15;
    const int HandleSideLength = SliderBarHeight+1;
    const int LeftRightMargin = 10;
    const int TopMargin = 10;
}

igQtColorRangeSlider::igQtColorRangeSlider(QWidget* aParent)
    : QWidget(aParent)
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明，好像没效果
    this->colorBarHeight = SliderBarHeight;
    setMouseTracking(true);
    initColorRanges();
}
void igQtColorRangeSlider::initColorRanges() {
    //auto iGameColorBar = iGame::iGameModelColorManager::Instance()->GetColorMapper();
    //if (this->myColorMap)delete this->myColorMap;
    //this->myColorMap = iGame::iGameColorMap::NewInstance();
    //auto tmpColorBar = this->myColorMap->GetColorBar();
    //auto tmpColorRange = this->myColorMap->GetColorRange();
    //tmpColorBar = iGame::iGameFloatArray::New();
    //tmpColorBar->SetNumberOfComponents(3);
    //tmpColorRange = iGame::iGameFloatArray::New();
    //int n = iGameColorBar->GetColorBarSize() + 1;
    //tmpColorBar->Resize(n);
    //tmpColorRange->Resize(n);
    //float rgb[16];
    //for (int i = 0; i < n; i++) {
    //    iGameColorBar->GetColorBar()->GetTuple(i, rgb);
    //    tmpColorBar->AddTuple(rgb);
    //}
    //for (int i = 0; i < n; i++) {
    //    tmpColorRange->AddValue(iGameColorBar->GetColorRange()->GetValue(i));
    //}
}
void igQtColorRangeSlider::updateColorBarDrawInfo() {

    //// 计算colors
    //this->colorBarLength = this->width() - 2 * LeftRightMargin;
    //drawColors.resize(colorBarLength);
    //float scale = 1.0 / colorBarLength;
    //QColor color;
    //float rgb[16];
    //for (int i = 0; i < colorBarLength; i++) {
    //    float val = i * scale;
    //    this->myColorMap->MapColor(val, rgb);
    //    color.setRgbF(rgb[0], rgb[1], rgb[2]);
    //    drawColors[i] = color;
    //}
}

void igQtColorRangeSlider::paintEvent(QPaintEvent* aEvent)
{
 /*   Q_UNUSED(aEvent);
    updateColorBarDrawInfo();
    QPainter painter(this);
    int height = this->height();
    int width = this->width();
    for (int i = 0; i < colorBarLength; i++) {
        QRect rect(LeftRightMargin + i, TopMargin, 1, colorBarHeight);
        painter.fillRect(rect, drawColors.at(i));
    }
    QPen pen;
    painter.setRenderHint(QPainter::Antialiasing);
    QBrush handleBrush(QColor(0xFA, 0xFA, 0xFA));
    painter.setBrush(handleBrush);
    auto colorRanges = this->myColorMap->GetColorRange();
    for (int i = 0; i < colorRanges->GetNumberOfTuples(); i++) {
        if (i == PressedHandle) {
            pen.setColor(QColor(155, 0, 0));
            pen.setWidth(4);
            painter.setPen(pen);
        }
        else {
            pen.setColor(Qt::darkGray);
            pen.setWidth(0.5);
            painter.setPen(pen);
        }
        QRectF HandleRect = getHandleRectWithFloatValue(colorRanges->GetValue(i));
        painter.drawRoundedRect(HandleRect, 4, 4);
    }*/
}

QRectF igQtColorRangeSlider::getHandleRectWithFloatValue(float aValue) const
{
    return getHandleRect(aValue * colorBarLength - HandleSideLength / 2 + LeftRightMargin);
}

QRectF igQtColorRangeSlider::getHandleRect(int aValue) const
{
    return QRect(aValue, TopMargin, HandleSideLength, HandleSideLength);
}

void igQtColorRangeSlider::mousePressEvent(QMouseEvent* aEvent)
{
    //this->isPressed = true;
    //int tmp = this->PressedHandle;
    //this->PressedHandle = -1;
    //auto colorRanges = this->myColorMap->GetColorRange();
    //auto colorBars = this->myColorMap->GetColorBar();
    //if (aEvent->buttons() & Qt::LeftButton) {
    //    int  posValue;
    //    posValue = aEvent->pos().x();
    //    for (int i = 0; i < colorRanges->GetNumberOfTuples(); i++) {
    //        if (getHandleRectWithFloatValue(colorRanges->GetValue(i)).contains(aEvent->pos())) {
    //            if (PressedHandle != -1) {
    //                if (abs(getHandleRectWithFloatValue(colorRanges->GetValue(i)).center().x() - posValue)
    //                    < abs(getHandleRectWithFloatValue(colorRanges->GetValue(PressedHandle)).center().x() - posValue)) {
    //                    PressedHandle = i;
    //                }
    //            }
    //            else {
    //                PressedHandle = i;
    //            }
    //        }
    //    }
    //    //std::cout << PressedHandle << '\n';
    //    if (PressedHandle != -1) {
    //        mDelta = posValue - getHandleRectWithFloatValue(colorRanges->GetValue(PressedHandle)).center().x();
    //    }
    //    if (tmp != PressedHandle) {
    //        update();
    //    }

    //}
    //else if (aEvent->buttons() & Qt::RightButton) {
    //    int  posValue;
    //    posValue = aEvent->pos().x();
    //    float rangeValue = (posValue - LeftRightMargin) * 1.0 / colorBarLength;
    //    if (rangeValue <= 0.001 || rangeValue >= 0.999)return;
    //    int idx = 0;
    //    for (int i = 1; i < colorRanges->GetNumberOfTuples(); i++) {
    //        if (colorRanges->GetValue(i - 1) < rangeValue && colorRanges->GetValue(i) > rangeValue) {
    //            idx = i; break;
    //        }
    //    }
    //    float rgb[3] = { 0 };
    //    myColorMap->MapColor(rangeValue, rgb);
    //    myColorMap->AddColorRange(idx, rangeValue);
    //    myColorMap->AddColorBar(idx, rgb);
    //    PressedHandle = idx;
    //    update();
    //}
    //else if (aEvent->buttons() & Qt::MiddleButton) {
    //    int  posValue;
    //    posValue = aEvent->pos().x();
    //    for (int i = 0; i < colorRanges->GetNumberOfTuples(); i++) {
    //        if (getHandleRectWithFloatValue(colorRanges->GetValue(i)).contains(aEvent->pos())) {
    //            if (PressedHandle != -1) {
    //                if (abs(getHandleRectWithFloatValue(colorRanges->GetValue(i)).center().x() - posValue)
    //                    < abs(getHandleRectWithFloatValue(colorRanges->GetValue(PressedHandle)).center().x() - posValue)) {
    //                    PressedHandle = i;
    //                }
    //            }
    //            else {
    //                PressedHandle = i;
    //            }
    //        }
    //    }
    //    if (PressedHandle != -1) {
    //        myColorMap->DeleteIndexColor(PressedHandle);
    //        PressedHandle = -1;
    //        Q_EMIT rangeChanges();
    //        update();
    //    }
    //}

}

void igQtColorRangeSlider::mouseMoveEvent(QMouseEvent* aEvent)
{
    //auto colorRanges = this->myColorMap->GetColorRange();
    //if (aEvent->buttons() & Qt::LeftButton && isPressed)
    //{
    //    if (this->isPressed == false || PressedHandle == -1 || PressedHandle == 0 || PressedHandle == myColorMap->GetColorBarSize())return;
    //    int posValue = aEvent->pos().x();
    //    double minPos = PressedHandle == 0 ? 0.0 : colorRanges->GetValue(PressedHandle - 1);
    //    double maxPos = PressedHandle == colorRanges->GetNumberOfTuples() - 1 ? 1.0 : colorRanges->GetValue(PressedHandle + 1);
    //    colorRanges->SetValue(PressedHandle, qBound(minPos, (posValue - mDelta - LeftRightMargin) * 1.0 / colorBarLength, maxPos));
    //    Q_EMIT rangeChanges();
    //    update();

    //}
}

void igQtColorRangeSlider::mouseReleaseEvent(QMouseEvent* aEvent)
{
    //Q_UNUSED(aEvent);
    //this->isPressed = false;
    //if (this->PressedHandle != -1) {
    //    QColor c;
    //    float rgb[3];
    //    this->myColorMap->GetColorBar()->GetTuple(PressedHandle, rgb);
    //    c.setRgbF(rgb[0], rgb[1], rgb[2]);
    //    Q_EMIT MouseReleased(c);
    //}

}
void igQtColorRangeSlider::updateColorInIndex(QColor c)
{
    //if (PressedHandle != -1) {
    //    this->myColorMap->SetColorBar(PressedHandle, c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0);
    //}
    //update();
    //Q_EMIT rangeChanges();
}

void igQtColorRangeSlider::updataManagerColorBarWithMyCorlorBar()
{
    //auto colorMap = iGame::iGameModelColorManager::Instance()->GetColorMapper();
    //int num = this->myColorMap->GetColorBar()->GetNumberOfTuples();
    //auto tmpColorBar = iGame::iGameFloatArray::New();
    //tmpColorBar->SetNumberOfComponents(3);
    //auto tmpColorRange = iGame::iGameFloatArray::New();
    //float rgb[3];
    //for (int i = 0; i < num; i++) {
    //    myColorMap->GetColorBar()->GetTuple(i, rgb);
    //    tmpColorBar->AddTuple(rgb);
    //    tmpColorRange->AddValue(myColorMap->GetColorRange()->GetValue(i));
    //}
    //colorMap->UpdateColorMap(tmpColorBar, tmpColorRange);


}
void igQtColorRangeSlider::changeColorBarWithDefaultMode(int mode)
{
    //switch (mode)
    //{
    //case 0:
    //    this->initColorRanges();
    //    break;
    //case 1:
    //    this->myColorMap->ResetColorBar(1);//灰度
    //    break;
    //case 2:
    //    this->myColorMap->ResetColorBar(2);//蓝红
    //    break;
    //case 3:
    //    this->myColorMap->ResetColorBar(4);//蓝绿黄红
    //    break;
    //case 4:
    //    this->myColorMap->ResetColorBar(5);//蓝绿黄红紫
    //    break;
    //default:
    //    break;
    //}
    //update();
    //Q_EMIT rangeChanges();
}

QSize igQtColorRangeSlider::minimumSizeHint() const
{
    return QSize(HandleSideLength * 2 + LeftRightMargin * 2, HandleSideLength);
}


