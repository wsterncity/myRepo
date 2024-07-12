//#pragma once
//
//#include "GLObject.h"
//#include "GLTexture2d.h"
//#include "GLTexture2dArray.h"
//#include "Renderbuffer.h"
//
//IGAME_NAMESPACE_BEGIN
//class Framebuffer : public GLObject<Framebuffer> {
//private:
//    friend class GLObject<Framebuffer>;
//    static void create(GLsizei count, GLuint* handles) {
//        glCreateFramebuffers(count, handles);
//    }
//    static void destroy(GLsizei count, GLuint* handles) {
//        glDeleteFramebuffers(count, handles);
//    }
//
//public:
//    void bind(GLenum target) const { glBindFramebuffer(target, handle); }
//
//    void draw_buffers(size_t count, GLenum* buffers) {
//        glNamedFramebufferDrawBuffers(handle, count, buffers);
//    }
//
//    void texture(GLenum attachment, const GLTexture2d& texture,
//                 unsigned mip_level) {
//        glNamedFramebufferTexture(handle, attachment, texture, mip_level);
//    }
//
//    void texture_layer(GLenum attachment, const GLTexture2dArray& texture,
//                       unsigned mip_level, unsigned layer) {
//        glNamedFramebufferTextureLayer(handle, attachment, texture, mip_level,
//                                       layer);
//    }
//
//    void renderbuffer(GLenum attachment, GLenum renderbuffer_target,
//                      const Renderbuffer& rbo) {
//        glNamedFramebufferRenderbuffer(handle, attachment, renderbuffer_target,
//                                       rbo);
//    }
//
//    GLenum check_status(GLenum target) {
//        return glCheckNamedFramebufferStatus(handle, target);
//    }
//};
//IGAME_NAMESPACE_END