#pragma once

#include "GLVendor.h"

IGAME_NAMESPACE_BEGIN
template<typename Helper>
class GLObject {
protected:
    GLuint handle;
    GLObject(GLuint _handle) : handle{_handle} {}

private:
    static GLuint createHandle() {
        GLuint handle;
        Helper::_create(1, &handle);
        return handle;
    }
    static void destroyHandle(GLuint& handle) {
        if (handle != 0) Helper::_destroy(1, &handle);
    }
    //static void destroyHandle() {
    //    if (handle != 0) Helper::_destroy(1, &handle);
    //}
public:
    GLObject() /*: handle{createHandle()}*/ {}

    GLObject(const GLObject&) = delete;
    GLObject(GLObject&& other) noexcept : handle{other.handle} {
        other.handle = 0;
    }

    GLObject& operator=(const GLObject&) = delete;
    GLObject& operator=(GLObject&& other) noexcept {
        destroyHandle(handle);
        handle = other.handle;
        other.handle = 0;

        return *this;
    };

    operator GLuint() const noexcept { return handle; }

    operator bool() const noexcept { return handle != 0; }

    void create() { handle = createHandle(); }
    void destroy() { destroyHandle(handle); }

    ~GLObject() { destroy(); }
};


IGAME_NAMESPACE_END