//
// Created by Sumzeek on 9/13/2024.
//

#pragma once

#include "iGameObject.h"

IGAME_NAMESPACE_BEGIN

// Only SolidLine has been implemented
enum class PenStyle {
    NoPen,
    SolidLine,
    //DashLine,
    //DotLine,
    //DashDotLine,
    //DashDotDotLine,
    //CustomDashLine
};

class Pen : public Object {
public:
    I_OBJECT(Pen);
    static Pointer New() { return new Pen; }

    void SetColor(const Color& color);
    Color GetColor() const;

    void SetWidth(int width);
    int GetWidth() const;

    void SetStyle(PenStyle style);
    PenStyle GetStyle() const;

    void SetOpacity(float opacity);
    float GetOpacity() const;

protected:
    Pen();
    ~Pen() override;

    Color m_PenColor;
    int m_PenWidth;
    PenStyle m_PenStyle;
    float m_PenOpacity;
};

IGAME_NAMESPACE_END