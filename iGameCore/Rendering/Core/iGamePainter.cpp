#include "iGamePainter.h"
#include "iGameScene.h"

IGAME_NAMESPACE_BEGIN

Painter::Painter() {

}

Vector3f Painter::ColorMap(Color c) {
    switch (c) {
        case Color::None:
        case Color::Red:
            return Vector3f(1, 0, 0);
        case Color::Green:
            return Vector3f(0, 1, 0);
        case Color::Blue:
            return Vector3f(0, 0, 1);
        case Color::LightBlue:
            return Vector3f(0.678, 0.847, 0.902);
        case Color::White:
            return Vector3f(1, 1, 1);
        default:
            return Vector3f(0, 0, 0);
    }
}
IGAME_NAMESPACE_END