#pragma once

#include "GLObject.h"

IGAME_NAMESPACE_BEGIN
class GLRenderbuffer : public GLObject<GLRenderbuffer> {
private:
    friend class GLObject<GLRenderbuffer>;
    static void createHandle(GLsizei count, GLuint* handles) {
        glGenRenderbuffers(count, handles);
    }
    static void destroyHandle(GLsizei count, GLuint* handles) {
        glDeleteRenderbuffers(count, handles);
    }

public:
    // GLenum internal_format: GL_DEPTH_COMPONENT32,  GL_DEPTH24_STENCIL8
    void storage(GLenum internal_format, unsigned width, unsigned height) {
        glBindRenderbuffer(GL_RENDERBUFFER, handle);
        glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
};
IGAME_NAMESPACE_END