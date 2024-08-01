#pragma once

#include "GLImageHandle.h"
#include "GLObject.h"
#include "GLTextureHandle.h"

IGAME_NAMESPACE_BEGIN

class GLTexture2d : public GLObject<GLTexture2d> {
private:
    friend class GLObject<GLTexture2d>;
    static void _create(GLsizei count, GLuint* handles) {
        glGenTextures(count, handles);
    }
    static void _destroy(GLsizei count, GLuint* handles) {
        glDeleteTextures(count, handles);
    }

private:
    GLTexture2d(GLuint handle) : GLObject<GLTexture2d>{handle} {}

public:
    static void copyImageSubData(const GLTexture2d& source, GLenum srcTarget,
                                 GLint srcLevel, GLint srcX, GLint srcY,
                                 GLint srcZ, const GLTexture2d& destination,
                                 GLenum dstTarget, GLint dstLevel, GLint dstX,
                                 GLint dstY, GLint dstZ, GLsizei srcWidth,
                                 GLsizei srcHeight, GLsizei srcDepth) {
        std::cout << "You called the GLTexture2d::copyImageSubData function on "
                     "the __APPLE__ "
                     "platform. This function is currently not supported."
                  << std::endl;
        //glCopyImageSubData(source.handle, srcTarget, srcLevel, srcX, srcY, srcZ,
        //                   destination.handle, dstTarget, dstLevel, dstX, dstY,
        //                   dstZ, srcWidth, srcHeight, srcDepth);
    }

public:
    GLTexture2d() = default;
    static GLTexture2d view(GLenum target, const GLTexture2d& original,
                            GLenum internal_format, unsigned first_mip_level,
                            unsigned mip_level_count, unsigned first_layer,
                            unsigned layer_count) {
        std::cout
                << "You called the GLTexture2d::view function on the __APPLE__ "
                   "platform. This function is currently not supported."
                << std::endl;
        //GLuint handle;
        //glGenTextures(1, &handle);
        //glTextureView(handle, target, original, internal_format,
        //              first_mip_level, mip_level_count, first_layer,
        //              layer_count);
        //return GLTexture2d(handle);
    }

public:
    // GLenum internal_format: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    // GLenum internal_format(Sized Internal Format): GL_R8, GL_RG8, GL_RGB8, GL_RGBA8
    // GLenum internal_format(Sized Internal Format): GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT24
    // GLenum internal_format(Sized Internal Format): GL_DEPTH32F_STENCIL8, GL_DEPTH24_STENCIL8
    // GLenum internal_format(Sized Internal Format): GL_STENCIL_INDEX8
    void storage(unsigned mip_levels, GLenum internal_format, unsigned width,
                 unsigned height) const {
        glBindTexture(GL_TEXTURE_2D, handle);

        GLenum format;
        GLenum type;

        // 根据 internal_format 选择适当的 format 和 type
        switch (internal_format) {
            case GL_R8:
                format = GL_RED;
                type = GL_UNSIGNED_BYTE;
                break;
            case GL_R32F:
                format = GL_RED;
                type = GL_FLOAT;
                break;
            case GL_RGB8:
                format = GL_RGB;
                type = GL_UNSIGNED_BYTE;
                break;
            case GL_DEPTH_COMPONENT32F:
                format = GL_DEPTH_COMPONENT;
                type = GL_FLOAT;
                break;
            default:
                std::cerr << "You called the GLTexture2d::storage function on "
                             "the "
                             "__APPLE__ platform, but the internal_format you "
                             "provided was not enumerated."
                          << std::endl;
                break;
        }

        // 分配存储空间
        for (unsigned int level = 0; level < mip_levels; ++level) {
            glTexImage2D(GL_TEXTURE_2D, level, internal_format, width >> level,
                         height >> level, 0, format, type, nullptr);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }


    // GLenum format(Base Internal Format): GL_RED, GL_RG, GL_RGB, GL_RGBA
    // GLenum type:GL_UNSIGNED_BYTE, GL_FLOAT
    void subImage(unsigned mip_level, unsigned xoffset, unsigned yoffset,
                  unsigned width, unsigned height, GLenum format, GLenum type,
                  const void* pixels) {
        glBindTexture(GL_TEXTURE_2D, handle);
        glTexSubImage2D(GL_TEXTURE_2D, mip_level, xoffset, yoffset, width,
                        height, format, type, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // GLenum pname: GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER
    // GLint param: GL_CLAMP_TO_EDGE, GL_MIRRORED_REPEAT, GL_NEAREST, GL_LINEAR
    void parameteri(GLenum pname, GLint param) {
        glBindTexture(GL_TEXTURE_2D, handle);
        glTexParameteri(GL_TEXTURE_2D, pname, param);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void generateMipmap() {
        glBindTexture(GL_TEXTURE_2D, handle);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

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
    void release() const { glBindTexture(GL_TEXTURE_2D, 0); }

    void bindImage(unsigned int binding_index, unsigned int mip_level,
                   bool layered, int layer, GLenum access, GLenum format) {
        std::cout << "You called the GLTexture2d::bindImage function on the "
                     "__APPLE__ "
                     "platform. This function is currently not supported."
                  << std::endl;
        //glBindImageTexture(binding_index, handle, mip_level, layered, layer,
        //                   access, format);
    }
};

IGAME_NAMESPACE_END