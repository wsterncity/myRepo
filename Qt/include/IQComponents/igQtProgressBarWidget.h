//
// Created by m_ky on 2024/5/22.
//

/**
 * @class   igQtProgressBarWidget
 * @brief   igQtProgressBarWidget's brief
 */
#pragma once

#include <QProgressBar>
#include <QLabel>
#include <QWidget>
#include <IQCore/igQtExportModule.h>

namespace iGame {
    class iGameProgressObserver;
}

class IG_QT_MODULE_EXPORT igQtProgressBarWidget : public QWidget{
public:
    static constexpr const char* DEFAULT = "Ready for";
    explicit igQtProgressBarWidget(QWidget *parent = nullptr);

    void updateProgressBar(double value);

    void updateProgressBarLabel(const char* info);
private:

    QProgressBar* progressBar;
    QLabel *progressBarLabel;
    iGame::iGameProgressObserver* ProgressObserver{ nullptr };
};