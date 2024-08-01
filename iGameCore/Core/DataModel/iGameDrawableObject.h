#ifndef iGameDrawableObject_h
#define iGameDrawableObject_h

#include "iGameObject.h"

#include "OpenGL/GLBuffer.h"
#include "OpenGL/GLShader.h"
#include "OpenGL/GLTexture2d.h"
#include "OpenGL/GLVertexArray.h"

IGAME_NAMESPACE_BEGIN
class Scene;
class DrawableObject {
public:
    DrawableObject() {}
    virtual ~DrawableObject() {}

    virtual void Draw(Scene*) = 0;
    virtual void ConvertToRenderableData() = 0;

    static void GLRequestContext() {}

    static void GLAllocateGLBuffer(GLBuffer& vbo, size_t size,
                                   const void* data) {
        IGAME_NAMESPACE::GLAllocateGLBuffer(vbo, size, data);
    }

    static void GLSetVertexAttrib(GLVertexArray& VAO,
                                  GLVertexAttribute attribute,
                                  GLuint vbo_binding_index, int size,
                                  GLenum type, GLboolean normalized,
                                  unsigned int offset) {
        IGAME_NAMESPACE::GLSetVertexAttrib(VAO, attribute, vbo_binding_index,
                                           size, type, normalized, offset);
    }
};
IGAME_NAMESPACE_END
#endif