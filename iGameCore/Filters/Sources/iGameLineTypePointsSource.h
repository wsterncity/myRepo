/**
 * @class   iGameLineTypePointsSource
 * @brief   iGameLineTypePointsSource's brief
 */

#pragma once

#include "iGamePointsSource.h"
#include "iGamePoints.h"

IGAME_NAMESPACE_BEGIN
class LineTypePointsSource : public PointSource{
public:
    I_OBJECT(LineTypePointsSource)
    static Pointer New(){ return new LineTypePointsSource;}

    bool Execute() override;

    void SetPoint_0(const float point0[3]);

    void SetPoint_1(const float point1[3]);


protected:
    LineTypePointsSource() = default;

    float m_Point_0[3];
    float m_Point_1[3];
};


IGAME_NAMESPACE_END