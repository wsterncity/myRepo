//
// Created by Sumzeek on 9/13/2024.
//

#include "iGamePen.h"

IGAME_NAMESPACE_BEGIN

Pen::Pen()
    : m_PenColor(Color::Black), m_PenWidth(1), m_PenStyle(PenStyle::SolidLine),
      m_PenOpacity(1.0f) {}

Pen::~Pen() {}

void Pen::SetColor(const Color& color) { m_PenColor = color; };

Color Pen::GetColor() const { return m_PenColor; }

void Pen::SetWidth(float width) { m_PenWidth = width; }

int Pen::GetWidth() const { return m_PenWidth; }

void Pen::SetStyle(PenStyle style) { m_PenStyle = style; }

PenStyle Pen::GetStyle() const { return m_PenStyle; }

//void Pen::SetOpacity(float opacity) { m_PenOpacity = opacity; }

//float Pen::GetOpacity() const { return m_PenOpacity; }

IGAME_NAMESPACE_END