#pragma once

#include "GLObject.h"
#include "GLRenderbuffer.h"
#include "GLTexture2d.h"
#include "GLTexture2dArray.h"
#include "GLTexture2dMultisample.h"

IGAME_NAMESPACE_BEGIN
class GLFramebuffer : public GLObject<GLFramebuffer> {
private:
    friend class GLObject<GLFramebuffer>;
    static void _create(GLsizei count, GLuint* handles) {
        glCreateFramebuffers(count, handles);
    }
    static void _destroy(GLsizei count, GLuint* handles) {
        glDeleteFramebuffers(count, handles);
    }

public:
    // GLenum target: GL_FRAMEBUFFER
    void bind(GLenum target) const { glBindFramebuffer(target, handle); }
    void release(GLenum target) const { glBindFramebuffer(target, 0); }

    void drawBuffers(size_t count, GLenum* buffers) {
        glNamedFramebufferDrawBuffers(handle, count, buffers);
    }

    // GLenum attachment: GL_COLOR_ATTACHMENT0(texture need GL_RGB)
    // GLenum attachment: GL_DEPTH_ATTACHMENT(texture need GL_DEPTH_COMPONENT)
    // GLenum attachment: GL_STENCIL_ATTACHMENT(texture need GL_STENCIL_INDEX)
    void texture(GLenum attachment, const GLTexture2d& texture,
                 unsigned mip_level) {
        glNamedFramebufferTexture(handle, attachment, texture, mip_level);
    }
    void texture(GLenum attachment, const GLTexture2dMultisample& texture,
                 unsigned mip_level) {
        glNamedFramebufferTexture(handle, attachment, texture, mip_level);
    }

    void textureLayer(GLenum attachment, const GLTexture2dArray& texture,
                      unsigned mip_level, unsigned layer) {
        glNamedFramebufferTextureLayer(handle, attachment, texture, mip_level,
                                       layer);
    }

    void renderbuffer(GLenum attachment, GLenum renderbuffer_target,
                      const GLFramebuffer& rbo) {
        glNamedFramebufferRenderbuffer(handle, attachment, renderbuffer_target,
                                       rbo);
    }

    GLenum checkStatus(GLenum target) {
        return glCheckNamedFramebufferStatus(handle, target);
    }
};
IGAME_NAMESPACE_END