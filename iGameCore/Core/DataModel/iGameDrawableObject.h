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
	DrawableObject(){}
	virtual ~DrawableObject() {}

	virtual void Draw(Scene*) = 0;
	virtual void ConvertToRenderableData() = 0;

	static void GLRequestContext() {

	}

	static void GLAllocateGLBuffer(GLBuffer& vbo, size_t size, const void* data) {
		vbo.allocate(size, data, GL_STATIC_DRAW);
	}

	static void GLSetVertexAttrib(GLVertexArray& VAO, GLVertexAttribute attribute,
		GLuint binding_index, int size, GLenum type,
		GLboolean normalized, unsigned int offset) {
		VAO.enableAttrib(attribute);
		VAO.attribBinding(attribute, binding_index);
		VAO.attribFormat(attribute, size, type, normalized, offset);
	}
};
IGAME_NAMESPACE_END
#endif