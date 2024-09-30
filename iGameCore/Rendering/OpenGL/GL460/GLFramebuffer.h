#pragma once

#include "GLObject.h"
#include "GLRenderbuffer.h"
#include "GLTexture2d.h"
#include "GLTexture2dArray.h"
#include "GLTexture2dMultisample.h"

IGAME_NAMESPACE_BEGIN
class GLFramebuffer : public GLObject<GLFramebuffer> {
private:
    GLenum m_Target;

private:
    friend class GLObject<GLFramebuffer>;
    static void createHandle(GLsizei count, GLuint* handles) {
        glCreateFramebuffers(count, handles);
    }
    static void destroyHandle(GLsizei count, GLuint* handles) {
        glDeleteFramebuffers(count, handles);
    }

public:
    // GLbitfield mask: GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_STENCIL_BUFFER_BIT
    // GLenum filter: GL_NEAREST, GL_LINEAR
    static void blit(const GLFramebuffer& source,
                     const GLFramebuffer& destination, GLint srcX0, GLint srcY0,
                     GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0,
                     GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {
        glBlitNamedFramebuffer(source.handle, destination.handle, srcX0, srcY0,
                               srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask,
                               filter);
    }

public:
    // GLenum target: GL_FRAMEBUFFER, GL_READ_FRAMEBUFFER, GL_DRAW_FRAMEBUFFER
    void target(GLenum target) { m_Target = target; }

    void bind() const { glBindFramebuffer(m_Target, handle); }
    void release() const { glBindFramebuffer(m_Target, 0); }

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
                      const GLRenderbuffer& rbo) {
        glNamedFramebufferRenderbuffer(handle, attachment, renderbuffer_target,
                                       rbo);
    }

    GLenum checkStatus() {
        return glCheckNamedFramebufferStatus(handle, m_Target);
    }
};
IGAME_NAMESPACE_END