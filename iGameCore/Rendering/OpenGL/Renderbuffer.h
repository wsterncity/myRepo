//#pragma once
//
//#include "GLObject.h"
//
//IGAME_NAMESPACE_BEGIN
//class Renderbuffer : public GLObject<Renderbuffer> {
//private:
//    friend class GLObject<Renderbuffer>;
//    static void create(GLsizei count, GLuint* handles) {
//        glCreateRenderbuffers(count, handles);
//    }
//    static void destroy(GLsizei count, GLuint* handles) {
//        glDeleteRenderbuffers(count, handles);
//    }
//
//public:
//    void storage(GLenum internal_format, unsigned width, unsigned height) {
//        glNamedRenderbufferStorage(handle, internal_format, width, height);
//    }
//};
//IGAME_NAMESPACE_END