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
    static Pointer New(){
        return new LineTypePointsSource();
    }

    bool Execute() override;

    void SetPoint_0(const Point& point0);

    void SetPoint_1(const Point& point1);

    void SetResolution(unsigned int Resolution);

protected:
    LineTypePointsSource(){};

    Point m_Point_0;
    Point m_Point_1;

    uint32_t m_Resolution{1};
};


IGAME_NAMESPACE_END