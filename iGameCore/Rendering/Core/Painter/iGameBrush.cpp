//
// Created by Sumzeek on 9/13/2024.
//

#include "iGameBrush.h"

IGAME_NAMESPACE_BEGIN

Brush::Brush()
    : m_BrushColor(Color::White), m_BrushStyle(BrushStyle::SolidPattern),
      m_BrushOpacity(1.0f) {}

Brush::~Brush() {}

void Brush::SetColor(const Color& color) { m_BrushColor = color; }

Color Brush::GetColor() const { return m_BrushColor; }

void Brush::SetStyle(BrushStyle style) { m_BrushStyle = style; }

BrushStyle Brush::GetStyle() const { return m_BrushStyle; }

//void Brush::SetOpacity(float opacity) { m_BrushOpacity = opacity; }

//float Brush::GetOpacity() const { return m_BrushOpacity; }

IGAME_NAMESPACE_END