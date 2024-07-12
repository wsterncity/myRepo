#pragma once

#include "GLObject.h"

IGAME_NAMESPACE_BEGIN

class GLBuffer : public GLObject<GLBuffer> {
private:
    friend class GLObject<GLBuffer>;
    static void _create(GLsizei count, GLuint* handles) {
        glCreateBuffers(count, handles);
    }
    static void _destroy(GLsizei count, GLuint* handles) {
        glDeleteBuffers(count, handles);
    }

public:
    static void copySubData(const GLBuffer& source, const GLBuffer& target,
                            size_t read_offset, size_t write_offset,
                            size_t size) {
        glCopyNamedBufferSubData(source.handle, target.handle, read_offset,
                                 write_offset, size);
    }

public:
    // GLenum usage: GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW
    // GLenum usage: GL_STATIC_READ, GL_DYNAMIC_READ, GL_STREAM_READ
    // GLenum usage: GL_STATIC_COPY, GL_DYNAMIC_COPY, GL_STREAM_COPY
    void allocate(size_t size, const void* data, GLenum usage) {
        glNamedBufferData(handle, size, data, usage);
    }

    void storage(size_t size, const void* data, GLbitfield flags) {
        glNamedBufferStorage(handle, size, data, flags);
    }

    void* mapRange(size_t offset, size_t length, GLbitfield access) const {
        return glMapNamedBufferRange(handle, offset, length, access);
    }

    void subData(size_t offset, size_t size, const void* data) const {
        glNamedBufferSubData(handle, offset, size, data);
    }

    void getSubData(size_t offset, size_t size, void* data) const {
        glGetNamedBufferSubData(handle, offset, size, data);
    }

    void unmap() {
        if (!glUnmapNamedBuffer(handle))
            throw std::runtime_error(
                    "data store contents have become corrupt during the time "
                    "the data store was mapped");
    }

    // What is binding this buffer for
    // GLenum target: GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_UNIFORM_BUFFER, GL_SHADER_STORAGE_BUFFER,GL_DRAW_INDIRECT_BUFFER,GL_DISPATCH_INDIRECT_BUFFER
    void bind(GLenum target) const { glBindBuffer(target, handle); }
    void release(GLenum target) const { glBindBuffer(target, 0); }

    void bindBase(GLenum target, unsigned index) const {
        glBindBufferBase(target, index, handle);
    }
};

static inline const unsigned int GL_VBO_IDX_0{0};
static inline const unsigned int GL_VBO_IDX_1{1};
static inline const unsigned int GL_VBO_IDX_2{2};
static inline const unsigned int GL_VBO_IDX_3{3};



IGAME_NAMESPACE_END