//
// Created by m_ky on 2024/8/8.
//

/**
 * @class   iGamePointDragInteractor
 * @brief   iGamePointDragInteractor's brief
 */
#pragma once

#include "iGameInteractor.h"
#include "iGamePointPicker.h"
#include "iGamePointSet.h"

IGAME_NAMESPACE_BEGIN
class PointDragInteractor : public Interactor {
public:
    I_OBJECT(PointDragInteractor)

    static Pointer New() { return new PointDragInteractor; }

    void MouseReleaseEvent(int _eventX, int _eventY) override;
    void MousePressEvent(int eventX, int eventY, MouseButton _mouseMode) override;

    void MouseMoveEvent(int posX, int posY) override;

    virtual void SetPointSet(PointSet::Pointer ps,
                             Model::Pointer model = nullptr);

protected:
    PointDragInteractor() = default;

    PointPicker::Pointer pointPicker{};
    PointSet::Pointer m_PointSet{};
    Model::Pointer m_Model{};
    igIndex Selected_Point_Index{-1};

    float Selected_NDC_Z{0};
};

IGAME_NAMESPACE_END