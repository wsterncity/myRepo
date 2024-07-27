#pragma once

#include "GLImageHandle.h"
#include "GLObject.h"
#include "GLTextureHandle.h"

IGAME_NAMESPACE_BEGIN

class GLTexture2d : public GLObject<GLTexture2d> {
private:
    friend class GLObject<GLTexture2d>;
    static void _create(GLsizei count, GLuint* handles) {
        glCreateTextures(GL_TEXTURE_2D, count, handles);
    }
    static void _destroy(GLsizei count, GLuint* handles) {
        glDeleteTextures(count, handles);
    }

private:
    GLTexture2d(GLuint handle) : GLObject<GLTexture2d>{handle} {}

public:
    GLTexture2d() = default;
    static GLTexture2d view(GLenum target, const GLTexture2d& original,
                            GLenum internal_format, unsigned first_mip_level,
                            unsigned mip_level_count, unsigned first_layer,
                            unsigned layer_count) {
        GLuint handle;
        glGenTextures(1, &handle);
        glTextureView(handle, target, original, internal_format,
                      first_mip_level, mip_level_count, first_layer,
                      layer_count);
        return GLTexture2d(handle);
    }

public:
    // GLenum internal_format: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    // Sized Internal Format: GL_R8, GL_RG8, GL_RGB8, GL_RGBA8
    void storage(unsigned mip_levels, GLenum internal_format, unsigned width,
                 unsigned height) const {
        glTextureStorage2D(handle, mip_levels, internal_format, width, height);
    }

    // GLenum format: GL_RED, GL_RG, GL_RGB, GL_RGBA
    // Base Internal Format: GL_RED, GL_RG, GL_RGB, GL_RGBA
    // GLenum type:GL_UNSIGNED_BYTE
    void subImage(unsigned mip_level, unsigned xoffset, unsigned yoffset,
                  unsigned width, unsigned height, GLenum format, GLenum type,
                  const void* pixels) {
        glTextureSubImage2D(handle, mip_level, xoffset, yoffset, width, height,
                            format, type, pixels);
    }

    // GLenum pname: GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER
    // GLint param: GL_CLAMP_TO_EDGE, GL_MIRRORED_REPEAT, GL_NEAREST, GL_LINEAR
    void parameteri(GLenum pname, GLint param) {
        glTextureParameteri(handle, pname, param);
    }

    void generateMipmap() { glGenerateTextureMipmap(handle); }

    // glUniformHandleui64ARB(glGetUniformLocation(shaderProgram,"tex0"), TextureHandle);
    GLTextureHandle getTextureHandle() const {
        return GLTextureHandle(glGetTextureHandleARB(handle));
    }

    GLImageHandle getImageHandle(unsigned level, bool layered, unsigned layer,
                                 GLenum format) const {
        return GLImageHandle(
                glGetImageHandleARB(handle, level, layered, layer, format));
    }

    void bind() const { glBindTexture(GL_TEXTURE_2D, handle); }
};

IGAME_NAMESPACE_END