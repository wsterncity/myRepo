//
// Created by Sumzeek on 9/13/2024.
//

#pragma once

#include "iGameObject.h"

IGAME_NAMESPACE_BEGIN

// Only SolidPattern has been implemented
enum class BrushStyle {
    NoBrush,
    SolidPattern,
    //Dense1Pattern,
    //Dense2Pattern,
    //Dense3Pattern,
    //Dense4Pattern,
    //Dense5Pattern,
    //Dense6Pattern,
    //Dense7Pattern,
    //HorPattern,
    //VerPattern,
    //CrossPattern,
    //BDiagPattern,
    //FDiagPattern,
    //DiagCrossPattern,
    //LinearGradientPattern,
    //RadialGradientPattern,
    //ConicalGradientPattern,
    //TexturePattern = 24
};

class Brush : public Object {
public:
    I_OBJECT(Brush);
    static Pointer New() { return new Brush; }

    void SetColor(const Color& color);
    Color GetColor() const;

    void SetStyle(BrushStyle style);
    BrushStyle GetStyle() const;

    //void SetOpacity(float opacity);
    //float GetOpacity() const;

protected:
    Brush();
    ~Brush() override;

    Color m_BrushColor;
    BrushStyle m_BrushStyle;
    float m_BrushOpacity;
};

IGAME_NAMESPACE_END