#ifndef OPENIGAME_SINGLE_DRAG_STYLE_H
#define OPENIGAME_SINGLE_DRAG_STYLE_H

#include "iGameSelectionStyle.h"
#include "iGamePointPicker.h"

IGAME_NAMESPACE_BEGIN
class SingleDragStyle : public SelectionStyle {
public:
    I_OBJECT(SingleDragStyle);
    static Pointer New() { return new SingleDragStyle; }

    void MousePressEvent(IEvent _event) override;
    void MouseMoveEvent(IEvent _event) override;

protected:
    SingleDragStyle() = default;
    ~SingleDragStyle() override = default;

    igIndex m_SelectedPointId{-1};

    float Selected_NDC_Z{0};
    igm::mat4 MVP{};
    igm::mat4 InvertedMVP{};
};
IGAME_NAMESPACE_END
#endif