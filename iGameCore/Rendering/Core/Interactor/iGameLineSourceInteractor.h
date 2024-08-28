//
// Created by m_ky on 2024/8/10.
//

/**
 * @class   iGameLineSourceInteractor
 * @brief   iGameLineSourceInteractor's brief
 */
#pragma once

#include "iGamePointDragInteractor.h"
#include "iGameFilter.h"

IGAME_NAMESPACE_BEGIN
class LineSourceInteractor : public PointDragInteractor{
public:
    I_OBJECT(LineSourceInteractor)

    static Pointer New(){ return new LineSourceInteractor;}

    void SetFilter(Filter::Pointer filter){
        m_Filter = filter;
    }


    void MouseMoveEvent(int posX, int posY) override {
        PointDragInteractor::MouseMoveEvent(posX, posY);
        if(m_Model != nullptr){
            m_Model->GetPointPainter()->Clear();
            for(int i = 0; i < m_PointSet->GetNumberOfPoints(); i ++)
                m_Model->GetPointPainter()->DrawPoint(m_PointSet->GetPoint(i));
        }
        if(~Selected_Point_Index && m_Filter != nullptr) m_Filter->Execute();
    }

protected:

    LineSourceInteractor(){}


    Filter::Pointer m_Filter{nullptr};
};

IGAME_NAMESPACE_END