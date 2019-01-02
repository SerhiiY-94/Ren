#include "Buffer.h"

#include <cassert>

#include <algorithm>

#if defined(USE_GL_RENDER)
#include "GL.h"
#elif defined(USE_SW_RENDER)
#include "SW/SW.h"
#endif

Ren::Buffer::Buffer(eBufferType type, uint32_t initial_size) : type_(type), size_(0) {
    nodes_.emplace_back();
    nodes_.back().size = initial_size;

#if defined(USE_GL_RENDER)
    buf_id_ = 0xffffffff;
#endif

    Resize(initial_size);
}

Ren::Buffer::~Buffer() {
#if defined(USE_GL_RENDER)
    if (type_ != UndefinedBuffer) {
        GLuint gl_buf = (GLuint)buf_id_;
        glDeleteBuffers(1, &gl_buf);
    }
#endif
}

Ren::Buffer &Ren::Buffer::operator=(Buffer &&rhs) {
    RefCounter::operator=(std::move(rhs));

    if (type_ != UndefinedBuffer) {
#if defined(USE_GL_RENDER)
        GLuint buf = (GLuint)buf_id_;
        glDeleteBuffers(1, &buf);
#endif
    }
    
    type_ = rhs.type_;
    rhs.type_ = UndefinedBuffer;

#if defined(USE_GL_RENDER) || defined(USE_SW_RENDER)
    buf_id_ = rhs.buf_id_;
#endif

    nodes_ = std::move(rhs.nodes_);

    size_ = rhs.size_;
    rhs.size_ = 0;

    return *this;
}

int Ren::Buffer::Alloc_Recursive(int i, uint32_t req_size) {
    if (!nodes_[i].is_free || req_size > nodes_[i].size) {
        return -1;
    }

    int ch0 = nodes_[i].child[0], ch1 = nodes_[i].child[1];

    if (ch0 != -1) {
        int new_node = Alloc_Recursive(ch0, req_size);
        if (new_node != -1) return new_node;

        return Alloc_Recursive(ch1, req_size);
    } else {
        if (req_size == nodes_[i].size) {
            nodes_[i].is_free = false;
            return i;
        }

        nodes_[i].child[0] = ch0 = (int)nodes_.size();
        nodes_.emplace_back();
        nodes_[i].child[1] = ch1 = (int)nodes_.size();
        nodes_.emplace_back();

        auto &n = nodes_[i];

        nodes_[ch0].offset = n.offset;
        nodes_[ch0].size = req_size;
        nodes_[ch1].offset = n.offset + req_size;
        nodes_[ch1].size = n.size - req_size;
        nodes_[ch0].parent = nodes_[ch1].parent = i;

        return Alloc_Recursive(ch0, req_size);
    }
}

int Ren::Buffer::Find_Recursive(int i, uint32_t offset) const {
    if ((nodes_[i].is_free && !nodes_[i].has_children()) ||
        offset < nodes_[i].offset || offset > (nodes_[i].offset + nodes_[i].size)) {
        return -1;
    }

    int ch0 = nodes_[i].child[0], ch1 = nodes_[i].child[1];

    if (ch0 != -1) {
        int ndx = Find_Recursive(ch0, offset);
        if (ndx != -1) return ndx;
        return Find_Recursive(ch1, offset);
    } else {
        if (offset == nodes_[i].offset) {
            return i;
        } else {
            return -1;
        }
    }
}

void Ren::Buffer::SafeErase(int i, int *indices, int num) {
    int last = (int)nodes_.size() - 1;

    if (last != i) {
        int ch0 = nodes_[last].child[0],
            ch1 = nodes_[last].child[1];

        if (ch0 != -1 && nodes_[i].parent != last) {
            nodes_[ch0].parent = nodes_[ch1].parent = i;
        }

        int par = nodes_[last].parent;

        if (nodes_[par].child[0] == last) {
            nodes_[par].child[0] = i;
        } else if (nodes_[par].child[1] == last) {
            nodes_[par].child[1] = i;
        }

        nodes_[i] = nodes_[last];
    }

    nodes_.erase(nodes_.begin() + last);

    for (int j = 0; j < num && indices; j++) {
        if (indices[j] == last) {
            indices[j] = i;
        }
    }
}

bool Ren::Buffer::Free_Node(int i) {
    if (i == -1 || nodes_[i].is_free) return false;

    nodes_[i].is_free = true;

    int par = nodes_[i].parent;
    while (par != -1) {
        int ch0 = nodes_[par].child[0], ch1 = nodes_[par].child[1];

        if (nodes_[ch0].has_children() && nodes_[ch0].is_free &&
            nodes_[ch1].has_children() && nodes_[ch1].is_free) {

            SafeErase(ch0, &par, 1);
            ch1 = nodes_[par].child[1];
            SafeErase(ch1, &par, 1);

            nodes_[par].child[0] = nodes_[par].child[1] = -1;

            par = nodes_[par].parent;
        }
    }

    return true;
}

uint32_t Ren::Buffer::Alloc(uint32_t req_size, void *init_data) {
    int i = Alloc_Recursive(0, req_size);
    if (i != -1) {
        auto &n = nodes_[i];
        assert(n.size == req_size);

        if (init_data) {
#if defined(USE_GL_RENDER)
            GLenum target = (type_ == VertexBuffer) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;

            glBindBuffer(target, (GLuint)buf_id_);
            glBufferSubData(target, n.offset, n.size, init_data);
#endif
        }
        
        return n.offset;
    } else {
        Resize(size_ + req_size);
        return Alloc(req_size);
    }
}

bool Ren::Buffer::Free(uint32_t offset) {
    int i = Find_Recursive(0, offset);
    return Free_Node(i);
}

void Ren::Buffer::Resize(uint32_t new_size) {
    if (size_ >= new_size) return;

    auto old_size = size_;

    if (!size_) size_ = new_size;

    while (size_ < new_size) {
        size_ *= 2;
    }

#if defined(USE_GL_RENDER)
    GLenum target = (type_ == VertexBuffer) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;

    GLuint gl_buffer;
    glGenBuffers(1, &gl_buffer);
    glBindBuffer(target, gl_buffer);
    glBufferData(target, size_, nullptr, GL_STATIC_DRAW);


    if (buf_id_ != 0xffffffff) {
        glBindBuffer(target, (GLuint)buf_id_);
        glBindBuffer(GL_COPY_WRITE_BUFFER, gl_buffer);

        glCopyBufferSubData(target, GL_COPY_WRITE_BUFFER, 0, 0, old_size);

        GLuint old_buffer = (GLuint)buf_id_;
        glDeleteBuffers(1, &old_buffer);

        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    }

    buf_id_ = (uint32_t)gl_buffer;
#else

#endif
}