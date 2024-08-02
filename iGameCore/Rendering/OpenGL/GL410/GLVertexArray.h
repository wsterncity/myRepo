#pragma once

#include "GLBuffer.h"
#include "GLObject.h"

IGAME_NAMESPACE_BEGIN

class GLVertexAttribute {
private:
    GLuint m_index;

public:
    explicit GLVertexAttribute(unsigned int location) : m_index{location} {}

    unsigned int index() const { return m_index; }
};

class GLVertexArray : public GLObject<GLVertexArray> {
private:
    friend class GLObject<GLVertexArray>;
    static void _create(GLsizei count, GLuint* handles) {
        glGenVertexArrays(count, handles);
    }
    static void _destroy(GLsizei count, GLuint* handles) {
        glDeleteVertexArrays(count, handles);
    }

public:
    void bind() const { glBindVertexArray(handle); }
    void release() const { glBindVertexArray(0); }

    void vertexBuffer(unsigned int vbo_binding_index, GLBuffer& buffer,
                      ptrdiff_t offset, size_t stride) {
        if (offset != 0) {
            throw std::runtime_error(
                    "You are trying to offset the VBO in the opengl410 "
                    "version, which is illegal. Please check your code.");
        }
        m_VBO[vbo_binding_index] = &buffer;
        m_VBOStride[vbo_binding_index] = stride;
    }

    void elementBuffer(GLBuffer& buffer) {
        glBindVertexArray(handle);
        buffer.target(GL_ELEMENT_ARRAY_BUFFER);
        buffer.bind();
        glBindVertexArray(0);
    }

    void enableAttrib(GLVertexAttribute attribute) {
        glBindVertexArray(handle);
        glEnableVertexAttribArray(attribute.index());
        glBindVertexArray(0);
    }

    void attribBindingFormat(GLVertexAttribute attribute,
                             unsigned int vbo_binding_index, int size,
                             GLenum type, bool normalized,
                             unsigned int relative_offset) {
        glBindVertexArray(handle);
        m_VBO[vbo_binding_index]->target(GL_ARRAY_BUFFER);
        m_VBO[vbo_binding_index]->bind();
        glVertexAttribPointer(attribute.index(), size, type, normalized,
                              m_VBOStride[vbo_binding_index],
                              (void*) relative_offset);
        glBindVertexArray(0);
    }

private:
    std::map<unsigned int, GLBuffer*> m_VBO;
    std::map<unsigned int, size_t> m_VBOStride;
};

//Per-vertex attributes binding indices
inline const GLVertexAttribute GL_LOCATION_IDX_0{0};
inline const GLVertexAttribute GL_LOCATION_IDX_1{1};
inline const GLVertexAttribute GL_LOCATION_IDX_2{2};
inline const GLVertexAttribute GL_LOCATION_IDX_3{3};

inline void GLAllocateGLBuffer(GLBuffer& vbo, size_t size, const void* data) {
    vbo.allocate(size, data, GL_STATIC_DRAW);
}

inline void GLSetVertexAttrib(GLVertexArray& VAO, GLVertexAttribute attribute,
                              GLuint vbo_binding_index, int size, GLenum type,
                              GLboolean normalized, unsigned int offset) {
    VAO.enableAttrib(attribute);
    VAO.attribBindingFormat(attribute, vbo_binding_index, size, type,
                            normalized, offset);
}

IGAME_NAMESPACE_END